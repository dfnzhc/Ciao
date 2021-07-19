#pragma once
#include "Core/Common.h"

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
        
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        
        bool Create(const WindowProps& props = WindowProps());

        void Shutdown();
        void HadleEvents();

        void BeginRender();
        void EndRender();
    
    private:
        SDL_Window* m_Window;
        SDL_GLContext m_GLContext;
        
        uint32_t m_Width;
        uint32_t m_Height;
    };
    
}