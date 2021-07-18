#include "Application.h"
#include "Scence.h"
#include "Window.h"
#include "Log.h"

#include <Renderer/RenderManager.h>

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
        : m_pScence(nullptr), m_isRunning(false), m_pWindow(nullptr)
    {
        m_pWindow = new Window;
        m_renderMgr = new RenderManager;
    }

    Application::~Application()
    {
        delete m_pWindow;
        delete m_renderMgr;
        
        m_pWindow = nullptr;
    }

    void Application::Execute(Scence* scence)
    {
        m_pScence = scence;
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
            
            WindowProps props = m_pScence->GetWindowProps();
            if (m_pWindow->Create(props)) {
                CIAO_CORE_INFO("Window initializing SUCCESSED..");
                m_renderMgr->Init();
                
                succ = true;
                m_isRunning = true;

                m_pScence->Init();
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

        m_renderMgr->Shutdown();
        m_pScence->Shutdown();
        m_pWindow->Shutdown();

        SDL_Quit();
    }

    void Application::Update()
    {
        m_pWindow->HadleEvents();
        m_pScence->Update();
    }

    void Application::Render()
    {
        m_pWindow->BeginRender();
        m_pScence->Render();
        m_pWindow->EndRender();
    }

    
}
