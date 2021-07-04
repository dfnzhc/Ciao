#include "pch.h"
#include "Application.h"
#include "Common.h"

static App* GameInst = nullptr;

App& App::GetInst()
{
    if (GameInst)
        return *GameInst;

    throw std::runtime_error("Error");
}

App::App()
{
    GameInst = this;
    m_wWidth = CommonWindowSize[0].Width;
    m_wHeight = CommonWindowSize[0].Height;
    m_dt = 0.0;
    m_elapsedTime = 0.0;
    m_appState = AppState::RUNNING;

    InitGlfwWindow();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Error");
    }
    glViewport(0, 0, m_wWidth, m_wHeight);
}

App::~App()
{
}

void App::InitGlfwWindow()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    m_pGlfwWindow = glfwCreateWindow(m_wWidth, m_wHeight, "LearnOpenGL", NULL, NULL);
    
    glfwMakeContextCurrent(m_pGlfwWindow);
    if (m_pGlfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Error");
    }

    // Set the required callback functions
    glfwSetKeyCallback(m_pGlfwWindow, key_callback);
}

void App::Init()
{
}

void App::Update()
{
    if (glfwWindowShouldClose(m_pGlfwWindow))
        m_appState = AppState::TERMINATE;
}

void App::Render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void App::Execute()
{
    Init();

    while (m_appState == AppState::RUNNING) {
        double t = glfwGetTime();
        m_dt = t - m_elapsedTime;
        m_elapsedTime = t;

        Render();
        Update();

        glfwSwapBuffers(m_pGlfwWindow);

        glfwPollEvents();
    }
    
    glfwTerminate();
}

