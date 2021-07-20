#pragma once

#include "ImguiWindow.h"

struct SDL_Window;
using SDL_GLContext = void*;

namespace Ciao {
    
    struct WindowProps
    {
        std::string Title;
        int XPos, YPos, Width, Height;
        int WindowFlags;
        ImguiWindowProperties ImguiProps;

        WindowProps();
    };

    class Window
    {
    public:
        Window();
        ~Window();
        
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        
        bool Create(const WindowProps& props = WindowProps());

        void Shutdown();
        void HadleEvents();

        void BeginRender();
        void EndRender();

        inline SDL_Window* GetSDLWindow() { return m_Window; }
        inline SDL_GLContext GetSDLContext() { return m_GLContext; }
    
    private:
        SDL_Window* m_Window;
        SDL_GLContext m_GLContext;

        std::shared_ptr<ImguiWindow> m_ImguiWindow;
        
        uint32_t m_Width;
        uint32_t m_Height;
    };
    
}