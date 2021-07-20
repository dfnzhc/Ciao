#pragma once

typedef union SDL_Event SDL_Event;
namespace Ciao
{
    struct ImguiWindowProperties
    {
        bool MoveFromTitleBarOnly = true;
        bool IsDockingEnabled = false;
        bool IsViewportEnabled = false;
    };
    
    class ImguiWindow
    {
    public:
        ImguiWindow() {}
        ~ImguiWindow() {}

        void Create(const ImguiWindowProperties& props);
        void Shutdown();

        void HandleSDLEvent(SDL_Event& e);

        void BeginRender();
        void EndRender();

        bool CaptureMouse();
    };
}


