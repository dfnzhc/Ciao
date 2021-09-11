#include "pch.h"
#include "Application.h"
#include "Scence.h"
#include "Window.h"

#include "Renderer/RenderManager.h"

#include "Mouse.h"
#include "Camera.h"


namespace Ciao
{ 
    Application* Application::m_pAppInst = nullptr;
    
    Application& Application::GetInst()
    {
        if (m_pAppInst == nullptr) {
            m_pAppInst = new Application();
        }
        return *m_pAppInst;
    }

    Application::Application()
        : m_Scence(nullptr), m_isRunning(false), m_Window(nullptr)
    {
        m_Window    = CreateRef<Window>();
        m_renderMgr = CreateRef<RenderManager>();
        m_Camera    = CreateRef<Camera>();
    }

    Application::~Application()
    {
    }

    void Application::Execute(Scence* scence)
    {
        m_Scence = std::shared_ptr<Scence>(scence);
        if (Init()) {
            while (m_isRunning) {
                Update();
                Render();
            }
            
            Terminate();
        }
    }

    void Application::GetWindowSize(uint32_t& w, uint32_t& h)
    {
        if (m_Window) {
            w = m_Window->GetWidth();
            h = m_Window->GetHeight();
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
            
            WindowProps props = m_Scence->GetWindowProps();
            if (m_Window->Create(props)) {
                CIAO_CORE_INFO("Window initializing SUCCESSED..");
                m_renderMgr->Init();
                
                succ = true;
                m_isRunning = true;

                Mouse::Init();
                m_Camera->Init();
                m_Camera->SetProjectionMatrix(45.0f,
                    (float)m_Window->GetWidth() / (float)m_Window->GetHeight(),
                    0.1f, 1000.0f);

                m_Scence->Init();
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
        m_Scence->Shutdown();
        m_Window->Shutdown();

        SDL_Quit();
    }

    void Application::Update()
    {
        m_Window->HadleEvents();
        m_Camera->Update();
        m_Scence->Update();
    }

    void Application::Render()
    {
        m_Window->BeginRender();
        m_Scence->Render();
        m_Window->EndRender();
    }

    
}
