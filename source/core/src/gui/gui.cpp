#include "gui.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "system/subsystem.h"
#include <event/frame_event.h>

namespace core
{
    core::Gui::Gui() { connect<FrameUiRender, Gui, &Gui::ui_renderer>(*this); }

    core::Gui::~Gui() { disconnect(*this); }

    void Gui::ui_renderer(const FrameUiRender& event)
    {
        // Our state
        static bool show_demo_window    = false;
        static bool show_another_window = false;

        // Main loop
        ImGuiIO& io = ImGui::GetIO();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
        // ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f       = 0.0f;
            static int   counter = 0;

            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
    }
} // namespace core
