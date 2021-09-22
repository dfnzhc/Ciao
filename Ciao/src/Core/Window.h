#pragma once

#include "ImguiWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Ciao {
    
    struct WindowProps
    {
        std::string Title;
        int XPos, YPos, Width, Height;
        int WindowFlags;
        ImguiWindowProperties ImguiProps;

        WindowProps();
    };

    class Window
    {
    public:
        Window();
        ~Window();
        
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        
        bool Create(const WindowProps& props = WindowProps());

        void Shutdown();
        void HadleEvents();

        void BeginRender();
        void EndRender();

        inline GLFWwindow* GetWindow() { return window_; }
    
    private:
        GLFWwindow* window_ = nullptr;
        double timeStamp_ = glfwGetTime();
        float deltaSeconds_ = 0;

        std::shared_ptr<ImguiWindow> m_ImguiWindow;
        
        uint32_t width_;
        uint32_t height_;
    };
    
}