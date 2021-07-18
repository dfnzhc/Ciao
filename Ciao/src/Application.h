#pragma once

namespace Ciao
{
    class Scence;
    class Window;
    
    class Application
    {
    public:
        static Application& GetAppInst();
        
        ~Application();

        void Execute(Scence* scence);
        inline void Shutdown() { m_isRunning = false; }
    
    private:
        bool Init();
        void Terminate();
        void Update();
        void Render();

    private:
        Window* m_pWindow;
        Scence* m_pScence;

        bool m_isRunning;

        Application();
        static Application* m_pAppInst;
    };
    
}