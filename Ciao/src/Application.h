#pragma once
#include <memory>

namespace Ciao
{
    class Scence;
    class Window;
    class RenderManager;
    
    class Application
    {
    public:
        static Application& GetAppInst();
        
        ~Application();

        void Execute(Scence* scence);
        inline void Shutdown() { m_isRunning = false; }
        
        inline RenderManager* GetRenderManager() { return m_renderMgr; }
    private:
        bool Init();
        void Terminate();
        void Update();
        void Render();

    private:
        Window* m_pWindow;
        Scence* m_pScence;
        RenderManager* m_renderMgr;

        bool m_isRunning;

        Application();
        static Application* m_pAppInst;
    };
    
}
