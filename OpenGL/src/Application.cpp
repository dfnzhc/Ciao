#include "pch.h"
#include "Application.h"
#include "Common.h"
#include "Shader.h"

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
    m_bgColor = glm::vec4{0.45f, 0.55f, 0.60f, 1.00f};
    m_triColor = glm::vec4{0.0f};
    m_pShaderProgram = nullptr;
    m_VAO = 0;

    InitGlfwWindow();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Error");
    }
    glViewport(0, 0, m_wWidth, m_wHeight);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("HarmonyOS_Sans_SC_Bold.ttf", 18.0f,
        NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_pGlfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

App::~App()
{
    delete m_pShaderProgram;
}

void App::Init()
{
    m_pShaderProgram = new ShaderProgram;
    // Load and compile shaders 
    Shader shVertex, shFragment;	
    shVertex.LoadShader("resources\\shaders\\shader.vert", GL_VERTEX_SHADER);
    shFragment.LoadShader("resources\\shaders\\shader.frag", GL_FRAGMENT_SHADER);

    // Create shader program and add shaders
    m_pShaderProgram->CreateProgram();
    m_pShaderProgram->AddShaderToProgram(&shVertex);
    m_pShaderProgram->AddShaderToProgram(&shFragment);
    m_pShaderProgram->LinkProgram();


    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };
    GLuint VBO;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 
}

void App::Update()
{
    if (glfwWindowShouldClose(m_pGlfwWindow))
        m_appState = AppState::TERMINATE;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    {
        ImGui::Begin("Settings");
        ImGui::Text("FPS %.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        ImGui::ColorEdit3("背景颜色", (float*)&m_bgColor);
        ImGui::ColorEdit3("Triangle Color", (float*)&m_triColor);
        ImGui::End();
    }

}

void App::Render()
{
    // Rendering
    glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, m_bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pShaderProgram->UseProgram();
    m_pShaderProgram->SetUniform("inColor", glm::vec3(m_triColor.r, m_triColor.g, m_triColor.b));
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::Execute()
{
    Init();

    while (m_appState == AppState::RUNNING) {
        double t = glfwGetTime();
        m_dt = t - m_elapsedTime;
        m_elapsedTime = t;

        Update();
        Render();

        glfwSwapBuffers(m_pGlfwWindow);

        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
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