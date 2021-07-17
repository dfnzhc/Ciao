#pragma once
#include "Window.h"

namespace Ciao
{
    class Renderer
    {
    public:
        Renderer(){}
        ~Renderer(){}

        virtual WindowProps GetWindowProps() { return WindowProps(); }

        virtual void Init() {}
        virtual void Shutdown() {}

        virtual void Update() {}
        virtual void Render() {}
    private:
        
    };

    Renderer* CreateRenderer();
    
}