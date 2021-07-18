#pragma once
#include "Window.h"

namespace Ciao
{
    class Scence
    {
    public:
        Scence(){}
        virtual ~Scence(){}

        virtual WindowProps GetWindowProps() { return WindowProps(); }

        virtual void Init() {}
        virtual void Shutdown() {}

        virtual void Update() {}
        virtual void Render() {}
    private:
        
    };

    Scence* CreateScence();
    
}