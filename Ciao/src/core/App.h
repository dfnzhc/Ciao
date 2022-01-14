#pragma once
#include <SDL_video.h>


namespace Ciao
{
    class App
    {
    public:
        App() = default;
        ~App();

        void Init();

        void mainLoop();

        bool isRunning() const { return running; }

    private:
        void Shutdown();
        void update();
        void handleEvents();

    private:
        
        SDL_Window* window_ = nullptr;
        SDL_GLContext context_ = nullptr;

        bool running = true;
    };
}
