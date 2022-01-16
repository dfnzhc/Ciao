#pragma once

#include <imgui.h>
#include <imgui_internal.h>

namespace Ciao
{
    class ImGuiRenderer
    {
    public:
        ImGuiRenderer() = default;
        ~ImGuiRenderer();

        void init(GLFWwindow* window);
        void render();
        void BeginRender();

        bool CaptureMouse();
    };

    void imguiTextureWindowGL(const char* title, uint32_t texId);
}
