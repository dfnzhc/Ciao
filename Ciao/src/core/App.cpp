#include "pch.h"
#include "App.h"

#include <SDL.h>

#include "render/Mouse.h"

namespace Ciao
{
    App::~App()
    {
        Shutdown();
    }

    void App::Init()
    {
        Logger::Init();
        if (SDL_Init(SDL_INIT_EVERYTHING))
        {
            CIAO_CORE_ERROR("SDL initializeing FAILED: {}", SDL_GetError());
        }
        else
        {
            CIAO_CORE_INFO("SDL initializing SUCCESSED..");
            SDL_version version;
            SDL_VERSION(&version);
            CIAO_CORE_INFO("SDL version: {}.{}.{}", (int32_t)version.major, (int32_t)version.minor, (int32_t)version.patch);

            
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
            
            SDL_DisplayMode current;
            SDL_GetCurrentDisplayMode(0, &current);
            SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
            CIAO_CORE_INFO("Display #{}: current display mode is {}x{}px @ {}hz.", 0, current.w, current.h, current.refresh_rate);
            
            // TODO: 修改默认窗口的大小
            window_ = SDL_CreateWindow("GLSL PathTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 800, window_flags);
            
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            
            context_ = SDL_GL_CreateContext(window_);
            if (!context_)
            {
                CIAO_CORE_ERROR("Failed to initialize GL context!");
            }
            
            // 禁用同步刷新
            SDL_GL_SetSwapInterval(0);
            
            // 加载 glad
            if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
            {
                CIAO_CORE_ERROR("Failed to initialize GL context!");
            }

            // TODO：初始化 imgui & ImGuizmo

        	// 初始化鼠标
        	Mouse::Init();
        }
    }

    void App::Shutdown()
    {
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    void App::mainLoop()
    {
    	handleEvents();
    	update();
    }

    void App::update()
    {
    	Mouse::Update();
    }

    void App::handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
        	//ImGui_ImplSDL2_ProcessEvent(&event);
        	if (event.type == SDL_QUIT)
        	{
        		running = false;
        	}
        	if (event.type == SDL_WINDOWEVENT)
        	{
        		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        		{
        			
        		}
        
        		if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window_))
        		{
        			running = false;
        		}
        	}
        }
    }
}
