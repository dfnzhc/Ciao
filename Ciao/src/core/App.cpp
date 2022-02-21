#include "pch.h"
#include "App.h"

#include "render/Camera.h"
#include "render/ImGuiRenderer.h"
#include "render/Mouse.h"

namespace Ciao
{
    App::App() : width_(800), height_(600), title_("We eat fish right.")
    {
        Logger::Init();

        glfwSetErrorCallback([](int error, const char* description)
        {
            CIAO_CORE_ERROR("Error: {}.", description);
        });

        if (!glfwInit())
        {
            CIAO_CORE_ERROR("Initialize GLFW Failed.");
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        const GLFWvidmode* info = glfwGetVideoMode(glfwGetPrimaryMonitor());
        
        // width_ = info->width;
        // height_ = info->height;
        window_ = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr);

        if (!window_)
        {
            CIAO_CORE_ERROR("Create GLFW window FAILED.");
            exit(EXIT_FAILURE);
        }

        CIAO_CORE_INFO("Create GLFW window SUCCESS!\t-{}x{}.", width_, height_);

        glfwSetWindowPos(window_, (info->width - width_) * 0.5f, (info->height - height_) * 0.5f);
        //glfwMaximizeWindow(window_);
        glfwMakeContextCurrent(window_);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            CIAO_CORE_ERROR("Failed to initialize GLAD");
            exit(EXIT_FAILURE);
        }

        glfwSwapInterval(0);

        glDebugMessageCallback(message_callback, nullptr);
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

        setCallback();


        // 初始化鼠标
        mouse_ = new Mouse();
        mouse_->Init();

        imgui_renderer_ = new ImGuiRenderer();
        imgui_renderer_->init(window_);
    }

    App::~App()
    {
        delete mouse_;
        delete imgui_renderer_;

        glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void App::swapBuffers()
    {
        imgui_renderer_->render();

        if (!imgui_renderer_->CaptureMouse())
            mouse_->Update(window_);

        glfwPollEvents();
        glfwSwapBuffers(window_);
        assert(glGetError() == GL_NO_ERROR);

        const double newTimeStamp = glfwGetTime();
        deltaSeconds_ = static_cast<float>(newTimeStamp - timeStamp_);
        timeStamp_ = newTimeStamp;
    }

    bool App::beginRender()
    {
        if (glfwWindowShouldClose(window_))
            return false;

        glfwGetFramebufferSize(window_, &width_, &height_);

        glViewport(0, 0, width_, height_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(.5f, .7f, .2f, 1.f);

        imgui_renderer_->BeginRender();

        return true;
    }

    void App::setCallback()
    {
        glfwSetScrollCallback(
            window_,
            [](GLFWwindow* window, double xoffset, double yoffset)
            {
                if (yoffset > 0)
                    CameraPositioner_Oribit::zoom(-0.5f);
                else if (yoffset < 0)
                    CameraPositioner_Oribit::zoom(0.5f);
            }
        );

        glfwSetKeyCallback(
            window_,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                const bool pressed = action != GLFW_RELEASE;
                if (key == GLFW_KEY_ESCAPE && pressed)
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        );

        glfwSetWindowSizeCallback(
            window_,
            [](GLFWwindow* window, int width, int height)
            {
                glViewport(0, 0, width, height);
            }
        );
    }

    void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message,
                          void const* user_param)
    {
        auto const src_str = [source]()
        {
            switch (source)
            {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            }
            return "";
        }();

        auto const type_str = [type]()
        {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            }
            return "";
        }();

        auto const severity_str = [severity]()
        {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            }
            return "";
        }();

        CIAO_CORE_INFO("{}, {}, {}, {}: {}.", src_str, type_str, severity_str, id, message);
    }
}
