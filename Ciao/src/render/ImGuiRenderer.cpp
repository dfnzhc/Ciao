#include "pch.h"
#include "ImGuiRenderer.h"

#include "utils/imgui/imgui_impl_glfw.h"
#include "utils/imgui/imgui_impl_opengl3.h"

namespace Ciao
{
    ImGuiRenderer::~ImGuiRenderer()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiRenderer::init(GLFWwindow* window)
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        ImFontConfig cfg = ImFontConfig();
        cfg.FontDataOwnedByAtlas = false;
        cfg.RasterizerMultiply = 1.5f;
        cfg.SizePixels = 17.0f;
        cfg.PixelSnapH = true;
        cfg.OversampleH = 4;
        cfg.OversampleV = 4;
        ImFont* Font = io.Fonts->AddFontFromFileTTF(Res("Fonts/HarmonyOS_Sans_SC_Bold.ttf"), cfg.SizePixels,
                                                    &cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

        (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiRenderer::render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiRenderer::BeginRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    bool ImGuiRenderer::CaptureMouse()
    {
        return ImGui::GetIO().WantCaptureMouse;
    }

    void imguiTextureWindowGL(const char* title, uint32_t texId)
    {
        ImGui::Begin(title, nullptr);

        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        ImGui::Image(
            (void*)(intptr_t)texId,
            ImVec2(vMax.x - vMin.x, vMax.y - vMin.y),
            ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
        );

        ImGui::End();
    }
}
