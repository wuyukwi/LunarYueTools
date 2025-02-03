#pragma once
#include "event/frame_event.h"
#include "imgui_impl_vulkan.h"
#include "renderer/renderer.h"
#include "vulkan/vulkan.h"
#include <SDL3/SDL_events.h>

namespace core
{
    class VulkanRenderer : public Renderer
    {
    public:
        VulkanRenderer() :
            allocator_(nullptr), instance_(VK_NULL_HANDLE), debugReport_(VK_NULL_HANDLE), physicalDevice_(VK_NULL_HANDLE), device_(VK_NULL_HANDLE),
            queueFamily_(static_cast<uint32_t>(-1)), queue_(VK_NULL_HANDLE), pipelineCache_(VK_NULL_HANDLE), descriptorPool_(VK_NULL_HANDLE)
        {
            Setup();
        }

        ~VulkanRenderer();
        void Setup() override;

        void pool_event(SDL_Event* event);

        void frame_update(float dt);
        void frame_render(float dt);

    protected:
        VkAllocationCallbacks*   allocator_;
        VkInstance               instance_;
        VkPhysicalDevice         physicalDevice_;
        VkDevice                 device_;
        uint32_t                 queueFamily_;
        VkQueue                  queue_;
        VkDebugReportCallbackEXT debugReport_;
        VkPipelineCache          pipelineCache_;
        VkDescriptorPool         descriptorPool_;

        ImGui_ImplVulkanH_Window mainWindowData_;
        uint32_t                 minImageCount_    = 2;
        bool                     swapChainRebuild_ = false;
        void                     Renderer();
        void                     SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
        void                     FramePresent();
        void                     CleanupVulkanWindow();
        void                     CleanupVulkan();
    };
} // namespace core
