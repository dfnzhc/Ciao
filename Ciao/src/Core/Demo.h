#pragma once
#include "Window.h"

namespace Ciao
{
    class Demo
    {
    public:
        Demo(){}
        virtual ~Demo(){}

        virtual WindowProps GetWindowProps() { return WindowProps(); }

        virtual void Init() {}
        virtual void Shutdown() {}

        virtual void Update() {}
        virtual void Render() {}
        virtual void ImguiRender() {}
    private:
        
    };

    Demo* CreateScence();
    
}