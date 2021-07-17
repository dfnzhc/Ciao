#include "Application.h"
#include "Renderer.h"
#include "Window.h"

#include "Log.h"

#include <SDL.h>

namespace Ciao
{
    Application* Application::m_pAppInst = nullptr;
    
    Application& Application::GetAppInst()
    {
        if (m_pAppInst == nullptr) {
            m_pAppInst = new Application();
        }
        return *m_pAppInst;
    }

    Application::Application()
        : m_pRenderer(nullptr), m_isRunning(false), m_pWindow(nullptr)
    {
        m_pWindow = new Window;
    }

    Application::~Application()
    {
        delete m_pWindow;
        m_pWindow = nullptr;
    }

    void Application::Execute(Renderer* renderer)
    {
        m_pRenderer = renderer;
        if (Init()) {
            while (m_isRunning) {
                Update();
                Render();
            }
            
            Terminate();
        }
    }

    bool Application::Init()
    {
        bool succ = false;

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            CIAO_CORE_ERROR("SDL initializing FAILD: {}", SDL_GetError());
        }
        else {
            CIAO_CORE_INFO("SDL initializing SUCCESSED..");
            SDL_version version;
            SDL_VERSION(&version);
            CIAO_CORE_INFO("SDL version: {}.{}.{}", (int32_t)version.major, (int32_t)version.minor, (int32_t)version.patch);
            
            WindowProps props = m_pRenderer->GetWindowProps();
            if (m_pWindow->Create(props)) {
                CIAO_CORE_INFO("Window initializing SUCCESSED..");
                
                succ = true;
                m_isRunning = true;

                m_pRenderer->Init();
            }
        }

        if (!succ) {
            CIAO_CORE_ERROR("Applicaition initializing FAILED. Terminating..");
            Terminate();
        }

        return succ;
    }

    void Application::Terminate()
    {
        m_isRunning = false;
        
        m_pRenderer->Shutdown();
        m_pWindow->Shutdown();

        SDL_Quit();
    }

    void Application::Update()
    {
        m_pWindow->HadleEvents();
        m_pRenderer->Update();
    }

    void Application::Render()
    {
        m_pWindow->BeginRender();
        m_pRenderer->Render();
        m_pWindow->EndRender();
    }

    
}
