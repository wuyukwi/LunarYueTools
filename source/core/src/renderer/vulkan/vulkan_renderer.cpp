#include "vulkan_renderer.h"
#include "imgui_impl_vulkan.h"
#include <algorithm>
#include <ranges>
#include <vector>

#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif
#include <window/window.h>

#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#endif

using namespace core;

static void check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension_name)
{
    return std::any_of(properties.begin(), properties.end(), [extension_name](const VkExtensionProperties& ext) {
        return std::strcmp(ext.extensionName, extension_name) == 0;
    });
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
}