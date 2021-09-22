#pragma once

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

        void BeginRender();
        void EndRender();

        bool CaptureMouse();
    };
}


