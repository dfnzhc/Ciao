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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_pGlfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");
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
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

