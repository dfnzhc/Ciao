#pragma once
#include "Common.h"

struct SDL_Window;
using SDL_GLContext = void*;

namespace Ciao {

    struct WindowProps
    {
        std::string Title;
        int XPos, YPos, Width, Height;
        int WindowFlags;

        WindowProps();
    };

    class Window
    {
    public:
        Window();
        ~Window();

        bool Create(const WindowProps& props);
        void Shutdown();

        void HadleEvents();

        void BeginRender();
        void EndRender();

    private:
        SDL_Window* m_pWindow;
        SDL_GLContext m_GLContext;
    };
    
}