#pragma once

namespace Ciao
{
    class Renderer;
    class Window;
    
    class Application
    {
    public:
        static Application& GetAppInst();
        
        ~Application();

        void Execute(Renderer* renderer);
        inline void Shutdown() { m_isRunning = false; }
    private:
        bool Init();
        void Terminate();
        void Update();
        void Render();


    private:
        Window* m_pWindow;
        Renderer* m_pRenderer;

        bool m_isRunning;

        Application();
        static Application* m_pAppInst;
    };
    
}