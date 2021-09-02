#include "pch.h"
#include "ImguiWindow.h"
#include "Application.h"

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include "Window.h"

namespace Ciao {
    void ImguiWindow::Create(const ImguiWindowProperties& props)
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = props.MoveFromTitleBarOnly;
        
        
        ImFontConfig cfg = ImFontConfig();
        cfg.RasterizerMultiply = 1.5f;
        cfg.PixelSnapH = true;
        cfg.OversampleH = 1;
        cfg.OversampleV = 1;
        
        std::string fontFile = GetAssetDir() + "Fonts\\HarmonyOS_Sans_SC_Bold.ttf";
        io.Fonts->AddFontFromFileTTF(fontFile.c_str(), 20.0f, &cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        (void)io;

        auto window = Application::GetInst().GetWindow();
        ImGui_ImplSDL2_InitForOpenGL(window->GetSDLWindow(), window->GetSDLContext());
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImguiWindow::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImguiWindow::HandleSDLEvent(SDL_Event& e)
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
    }

    void ImguiWindow::BeginRender()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(Application::GetInst().GetWindow()->GetSDLWindow());
        ImGui::NewFrame();
    }

    void ImguiWindow::EndRender()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    bool ImguiWindow::CaptureMouse()
    {
        return ImGui::GetIO().WantCaptureMouse;
    }
}
