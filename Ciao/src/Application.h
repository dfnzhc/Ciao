#pragma once
#include <memory>

#define CIAO_SUB_RC(x) Ciao::Application::GetInst().GetRenderManager()->Submit((x))

namespace Ciao
{
    class Demo;
    class Window;
    class Camera;
    
    class Application
    {
    public:
        static Application& GetInst();
        
        ~Application();

        void Execute(Demo* scence);
        
        inline std::shared_ptr<Camera> GetCamera() { return m_Camera; }
        inline std::shared_ptr<Window> GetWindow() { return m_Window; }
        inline std::shared_ptr<Demo> GetScence() { return m_Demo; }
        
        void GetWindowSize(uint32_t& w, uint32_t& h);
    private:
        bool Init();
        void Terminate();
        void Update();
        void Render();

        void SetGLFWCallBack();

    private:
        std::shared_ptr<Demo> m_Demo;
        std::shared_ptr<Camera> m_Camera;
        std::shared_ptr<Window> m_Window;

        Application();
        static Application* m_pAppInst;
    };
    
}
