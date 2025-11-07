#include "vulkan_renderer.h"
#include "imgui_impl_vulkan.h"
#include <algorithm>
#include <ranges>
#include <vector>

#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif
#include "event/frame_event.h"
#include "logger.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_vulkan.h>
#include <imgui_impl_sdl3.h>

#include <event/EventManager.h>
#include <event/sdl_event.h>
#include <window/window_manager.h>
namespace core
{
    static void check_vk_result(VkResult err)
    {
        if (err == VK_SUCCESS)
            return;
        APPLOG_ERROR("[vulkan] Error: VkResult = {}", (int)err);
        if (err < 0)
            abort();
    }

    static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension_name)
    {
        return std::any_of(properties.begin(), properties.end(), [extension_name](const VkExtensionProperties& ext) {
            return std::strcmp(ext.extensionName, extension_name) == 0;
        });
    }

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT      flags,
                                                       VkDebugReportObjectTypeEXT objectType,
                                                       uint64_t                   object,
                                                       size_t                     location,
                                                       int32_t                    messageCode,
                                                       const char*                pLayerPrefix,
                                                       const char*                pMessage,
                                                       void*                      pUserData)
    {
        (void)flags;
        (void)object;
        (void)location;
        (void)messageCode;
        (void)pUserData;
        (void)pLayerPrefix; // Unused arguments
        APPLOG_DEBUG("[vulkan] Debug report from ObjectType: {}\nMessage: {}", (int)objectType, pMessage);
        return VK_FALSE;
    }
#endif // APP_USE_VULKAN_DEBUG_REPORT

    VulkanRenderer::~VulkanRenderer()
    {
        // Cleanup
        auto err = vkDeviceWaitIdle(device_);
        check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        CleanupVulkanWindow();
        CleanupVulkan();
        disconnect(*this);
    }

    void core::VulkanRenderer::Setup()
    {
        VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
        volkInitialize();
#endif

        VkInstanceCreateInfo create_info = {};
        create_info.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        uint32_t                           properties_count;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.data());
        check_vk_result(err);

        std::vector<const char*> instance_extensions;
        uint32_t                 sdl_extensions_count = 0;
        const char* const*       sdl_extensions       = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
        instance_extensions.insert(instance_extensions.end(), sdl_extensions, sdl_extensions + sdl_extensions_count);

        // Enable required extensions
        if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            instance_extensions.emplace_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
        {
            instance_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif

        // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        const char* layers[]            = {"VK_LAYER_KHRONOS_validation"};
        create_info.enabledLayerCount   = 1;
        create_info.ppEnabledLayerNames = layers;
        instance_extensions.push_back("VK_EXT_debug_report");
#endif

        // Create Vulkan Instance
        create_info.enabledExtensionCount   = instance_extensions.size();
        create_info.ppEnabledExtensionNames = instance_extensions.data();
        err                                 = vkCreateInstance(&create_info, allocator_, &instance_);
        check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
        volkLoadInstance(instance_);
#endif

        // Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        auto f_vkCreateDebugReportCallbackEXT =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugReportCallbackEXT");
        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType                              = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData   = nullptr;
        err                         = f_vkCreateDebugReportCallbackEXT(instance_, &debug_report_ci, allocator_, &debugReport_);
        check_vk_result(err);
#endif
        // Select Physical Device (GPU)
        physicalDevice_ = ImGui_ImplVulkanH_SelectPhysicalDevice(instance_);

        // Select graphics queue family
        queueFamily_ = ImGui_ImplVulkanH_SelectQueueFamilyIndex(physicalDevice_);

        // Create Logical Device (with 1 queue)
        std::vector<const char*> device_extensions;
        device_extensions.push_back("VK_KHR_swapchain");

        {
            // Enumerate physical device extension
            uint32_t                           properties_count;
            std::vector<VkExtensionProperties> properties;
            vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &properties_count, nullptr);
            properties.resize(properties_count);
            vkEnumerateDeviceExtensionProperties(physicalDevice_, nullptr, &properties_count, properties.data());
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
            if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
                device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

            const float             queue_priority[] = {1.0f};
            VkDeviceQueueCreateInfo queue_info[1]    = {};
            queue_info[0].sType                      = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info[0].queueFamilyIndex           = queueFamily_;
            queue_info[0].queueCount                 = 1;
            queue_info[0].pQueuePriorities           = queue_priority;
            VkDeviceCreateInfo create_info           = {};
            create_info.sType                        = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            create_info.queueCreateInfoCount         = sizeof(queue_info) / sizeof(queue_info[0]);
            create_info.pQueueCreateInfos            = queue_info;
            create_info.enabledExtensionCount        = device_extensions.size();
            create_info.ppEnabledExtensionNames      = device_extensions.data();
            err                                      = vkCreateDevice(physicalDevice_, &create_info, allocator_, &device_);
            check_vk_result(err);
            vkGetDeviceQueue(device_, queueFamily_, 0, &queue_);
        }

        // Create Descriptor Pool
        // If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
        {
            VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
            };
            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets                    = 0;
            for (VkDescriptorPoolSize& pool_size : pool_sizes)
                pool_info.maxSets += pool_size.descriptorCount;
            pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
            pool_info.pPoolSizes    = pool_sizes;
            err                     = vkCreateDescriptorPool(device_, &pool_info, allocator_, &descriptorPool_);
            check_vk_result(err);
        }

        // Create Window Surface
        VkSurfaceKHR surface;
        auto         window = get_subsystem<WindowManager>().get_main_window()->get_sdl_window_ptr();
        if (SDL_Vulkan_CreateSurface(window, instance_, allocator_, &surface) == 0)
        {
            APPLOG_ERROR("Failed to create Vulkan surface.\n");
        }

        // Create Framebuffers
        ImGui_ImplVulkanH_Window* wd = &mainWindowData_;
        int                       w, h;
        SDL_GetWindowSize(window, &w, &h);
        SetupVulkanWindow(wd, surface, w, h);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_ShowWindow(window);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplSDL3_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance                  = instance_;
        init_info.PhysicalDevice            = physicalDevice_;
        init_info.Device                    = device_;
        init_info.QueueFamily               = queueFamily_;
        init_info.Queue                     = queue_;
        init_info.PipelineCache             = pipelineCache_;
        init_info.DescriptorPool            = descriptorPool_;
        init_info.RenderPass                = wd->RenderPass;
        init_info.Subpass                   = 0;
        init_info.MinImageCount             = minImageCount_;
        init_info.ImageCount                = wd->ImageCount;
        init_info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator                 = allocator_;
        init_info.CheckVkResultFn           = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);

        connect<FrameUpdate, VulkanRenderer, &VulkanRenderer::frame_update>(*this);
        connect<FrameRender, VulkanRenderer, &VulkanRenderer::frame_render>(*this);
        connect<SDLEvent, VulkanRenderer, &VulkanRenderer::pool_event>(*this);
    }

    void VulkanRenderer::pool_event(const SDLEvent& event) { ImGui_ImplSDL3_ProcessEvent(event.event); }

    void VulkanRenderer::frame_update(const FrameUpdate& dt)
    {
        // Resize swap chain?
        int fb_width, fb_height;
        get_subsystem<WindowManager>().get_size(fb_width, fb_height);
        if (fb_width > 0 && fb_height > 0 && (swapChainRebuild_ || mainWindowData_.Width != fb_width || mainWindowData_.Height != fb_height))
        {
            ImGui_ImplVulkan_SetMinImageCount(minImageCount_);
            ImGui_ImplVulkanH_CreateOrResizeWindow(
                instance_, physicalDevice_, device_, &mainWindowData_, queueFamily_, allocator_, fb_width, fb_height, minImageCount_);
            mainWindowData_.FrameIndex = 0;
            swapChainRebuild_          = false;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void VulkanRenderer::frame_render(const FrameRender& dt)
    {
        ImGui::Render();

        ImGuiIO&    io                = ImGui::GetIO();
        ImDrawData* main_draw_data    = ImGui::GetDrawData();
        const bool  main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        if (!main_is_minimized)
            Renderer();

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        // Present Main Platform Window
        if (!main_is_minimized)
            FramePresent();
    }

    void VulkanRenderer::Renderer()
    {
        ImDrawData* draw_data = ImGui::GetDrawData();

        VkSemaphore image_acquired_semaphore  = mainWindowData_.FrameSemaphores[mainWindowData_.SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = mainWindowData_.FrameSemaphores[mainWindowData_.SemaphoreIndex].RenderCompleteSemaphore;
        VkResult    err                       = vkAcquireNextImageKHR(
            device_, mainWindowData_.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &mainWindowData_.FrameIndex);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
            swapChainRebuild_ = true;
        if (err == VK_ERROR_OUT_OF_DATE_KHR)
            return;
        if (err != VK_SUBOPTIMAL_KHR)
            check_vk_result(err);

        ImGui_ImplVulkanH_Frame* fd = &mainWindowData_.Frames[mainWindowData_.FrameIndex];
        {
            err = vkWaitForFences(device_, 1, &fd->Fence, VK_TRUE, UINT64_MAX); // wait indefinitely instead of periodically checking
            check_vk_result(err);

            err = vkResetFences(device_, 1, &fd->Fence);
            check_vk_result(err);
        }
        {
            err = vkResetCommandPool(device_, fd->CommandPool, 0);
            check_vk_result(err);
            VkCommandBufferBeginInfo info = {};
            info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
            check_vk_result(err);
        }
        {
            VkRenderPassBeginInfo info    = {};
            info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass               = mainWindowData_.RenderPass;
            info.framebuffer              = fd->Framebuffer;
            info.renderArea.extent.width  = mainWindowData_.Width;
            info.renderArea.extent.height = mainWindowData_.Height;
            info.clearValueCount          = 1;
            info.pClearValues             = &mainWindowData_.ClearValue;
            vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // Submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        {
            VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo         info       = {};
            info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            info.waitSemaphoreCount         = 1;
            info.pWaitSemaphores            = &image_acquired_semaphore;
            info.pWaitDstStageMask          = &wait_stage;
            info.commandBufferCount         = 1;
            info.pCommandBuffers            = &fd->CommandBuffer;
            info.signalSemaphoreCount       = 1;
            info.pSignalSemaphores          = &render_complete_semaphore;

            err = vkEndCommandBuffer(fd->CommandBuffer);
            check_vk_result(err);
            err = vkQueueSubmit(queue_, 1, &info, fd->Fence);
            check_vk_result(err);
        }
    }

    // All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
    // Your real engine/app may not use them.
    void VulkanRenderer::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
    {
        wd->Surface = surface;

        // Check for WSI support
        VkBool32 res;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, queueFamily_, wd->Surface, &res);
        if (res != VK_TRUE)
        {
            fprintf(stderr, "Error no WSI support on physical device 0\n");
            exit(-1);
        }

        // Select Surface Format
        const VkFormat requestSurfaceImageFormat[] = {
            VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat                              = ImGui_ImplVulkanH_SelectSurfaceFormat(
            physicalDevice_, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

        // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
        VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(physicalDevice_, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

        // Create SwapChain, RenderPass, Framebuffer, etc.
        ImGui_ImplVulkanH_CreateOrResizeWindow(instance_, physicalDevice_, device_, wd, queueFamily_, allocator_, width, height, minImageCount_);
    }

    void VulkanRenderer::FramePresent()
    {
        if (swapChainRebuild_)
            return;
        VkSemaphore      render_complete_semaphore = mainWindowData_.FrameSemaphores[mainWindowData_.SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info                      = {};
        info.sType                                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount                    = 1;
        info.pWaitSemaphores                       = &render_complete_semaphore;
        info.swapchainCount                        = 1;
        info.pSwapchains                           = &mainWindowData_.Swapchain;
        info.pImageIndices                         = &mainWindowData_.FrameIndex;
        VkResult err                               = vkQueuePresentKHR(queue_, &info);
        if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
            swapChainRebuild_ = true;
        if (err == VK_ERROR_OUT_OF_DATE_KHR)
            return;
        if (err != VK_SUBOPTIMAL_KHR)
            check_vk_result(err);
        mainWindowData_.SemaphoreIndex =
            (mainWindowData_.SemaphoreIndex + 1) % mainWindowData_.SemaphoreCount; // Now we can use the next set of semaphores
    }

    void VulkanRenderer::CleanupVulkanWindow() { ImGui_ImplVulkanH_DestroyWindow(instance_, device_, &mainWindowData_, allocator_); }

    void VulkanRenderer::CleanupVulkan()
    {
        vkDestroyDescriptorPool(device_, descriptorPool_, allocator_);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
        // Remove the debug report callback
        auto f_vkDestroyDebugReportCallbackEXT =
            (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugReportCallbackEXT");
        f_vkDestroyDebugReportCallbackEXT(instance_, debugReport_, allocator_);
#endif // APP_USE_VULKAN_DEBUG_REPORT

        vkDestroyDevice(device_, allocator_);
        vkDestroyInstance(instance_, allocator_);
    }

} // namespace core