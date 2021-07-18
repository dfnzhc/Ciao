﻿#include "Window.h"
#include "Application.h"
#include "Log.h"

#include <glad/glad.h>
#include <SDL.h>
#include <glm.hpp>

#include "Renderer/RenderManager.h"

namespace Ciao
{
    WindowProps::WindowProps()
        : Title("Ciao Renderer"), XPos(SDL_WINDOWPOS_CENTERED), YPos(SDL_WINDOWPOS_CENTERED),
            Width(1600), Height(980), WindowFlags(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI)
    {
    
    }

    Window::Window() : m_pWindow(nullptr), m_Width(0), m_Height(0)
    {
    }

    Window::~Window()
    {
        if (m_pWindow) {
            Shutdown();
        }
    }

    uint32_t Window::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Window::GetHeight() const
    {
        return m_Height;
    }

    bool Window::Create(const WindowProps& props)
    {
        m_pWindow = SDL_CreateWindow(props.Title.c_str(), props.XPos, props.YPos, props.Width, props.Height, props.WindowFlags);
        if (!m_pWindow) {
            CIAO_CORE_ERROR("Create window FAILED: {}", SDL_GetError());
            return false;
        }

        m_Width = props.Width;
        m_Height = props.Height;
        CIAO_CORE_INFO("Create window SUCCESSED: Width - {} Height - {}", m_Width, m_Height);

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);
        CIAO_CORE_INFO("Display #{}: current display mode is {}x{}px @ {}hz.", 0, current.w, current.h, current.refresh_rate);

        // 设置窗口属性
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        m_GLContext = SDL_GL_CreateContext(m_pWindow);
        if (m_GLContext == nullptr) {
            CIAO_CORE_ERROR("Create OpenGL context FAILED: {}", SDL_GetError());
            return false;
        }
        
        gladLoadGLLoader(SDL_GL_GetProcAddress);
        

        return true;
    }

    void Window::Shutdown()
    {
        SDL_DestroyWindow(m_pWindow);
        SDL_GL_DeleteContext(m_GLContext);
        m_pWindow = nullptr;
    }

    void Window::HadleEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                // 窗口关闭
                Application::GetAppInst().Shutdown();
                break;
                
            default:
                break;
            }
        }
    }

    void Window::BeginRender()
    {
        auto rm = Application::GetAppInst().GetRenderManager();
        rm->Clear();
    }

    void Window::EndRender()
    {
        auto rm = Application::GetAppInst().GetRenderManager();
        rm->Flush();

        
        SDL_GL_SwapWindow(m_pWindow);
    }
} 