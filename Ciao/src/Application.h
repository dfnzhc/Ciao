#pragma once
#include <memory>

namespace Ciao
{
    class Scence;
    class Window;
    class RenderManager;
    class Camera;
    
    class Application
    {
    public:
        static Application& GetInst();
        
        ~Application();

        void Execute(Scence* scence);
        inline void Shutdown() { m_isRunning = false; }
        
        inline std::shared_ptr<RenderManager> GetRenderManager() { return m_renderMgr; }
        inline std::shared_ptr<Camera> GetCamera() { return m_Camera; }
        inline std::shared_ptr<Window> GetWindow() { return m_Window; }
        inline std::shared_ptr<Scence> GetScence() { return m_Scence; }
        
        void GetWindowSize(uint32_t& w, uint32_t& h);
    private:
        bool Init();
        void Terminate();
        void Update();
        void Render();

    private:
        std::shared_ptr<Scence> m_Scence;
        std::shared_ptr<RenderManager> m_renderMgr;
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Window> m_Window;

        bool m_isRunning;

        Application();
        static Application* m_pAppInst;
    };
    
}
