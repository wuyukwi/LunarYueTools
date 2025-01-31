#pragma once
#include "renderer/renderer.h"

namespace core
{
    class VulkanRenderer : public Renderer
    {
    public:
        void Setup() override;
    };
} // namespace core
