#include "pch.h"
#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Cube.h"
#include "Sphere.h"

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

    /// 属性
    m_dt = 0.0;
    m_elapsedTime = 0.0;
    m_appState = AppState::RUNNING;
    m_bgColor = glm::vec4{0.45f, 0.55f, 0.60f, 1.00f};
    m_triColor = glm::vec4{0.0f};
    m_pShaderProgram = nullptr;
    m_VAO = 0;

    /// 场景物体
    m_pCube = nullptr;
    m_pSphere = nullptr;
}

App::~App()
{
    delete m_pShaderProgram;
    delete m_pCube;
    delete m_pSphere;
}

void App::Init()
{
    /// 创建物体
    m_pShaderProgram = new ShaderProgram;
    m_pCube = new Cube;
    m_pSphere = new Sphere;
    
    // Load and compile shaders 
    Shader shVertex, shFragment;	
    shVertex.LoadShader("resources\\Shaders\\shader.vert", GL_VERTEX_SHADER);
    shFragment.LoadShader("resources\\Shaders\\shader.frag", GL_FRAGMENT_SHADER);

    // Create shader program and add shaders
    m_pShaderProgram->CreateProgram();
    m_pShaderProgram->AddShaderToProgram(&shVertex);
    m_pShaderProgram->AddShaderToProgram(&shFragment);
    m_pShaderProgram->LinkProgram();


    m_pCube->Create("resources\\Textures\\container.jpg");
    m_pSphere->Create("resources\\Textures\\container.jpg", 25, 25);
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
        //ImGui::ColorEdit3("Triangle Color", (float*)&m_triColor);
        ImGui::End();
    }

}

void App::Render()
{
    // Rendering
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_bgColor.x, m_bgColor.y, m_bgColor.z, m_bgColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pShaderProgram->UseProgram();
    // create transformations
    glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)m_wWidth / (float)m_wHeight, 0.1f, 100.0f);
    m_pShaderProgram->SetUniform("model", model);
    m_pShaderProgram->SetUniform("view", view);
    m_pShaderProgram->SetUniform("projection", projection);
    m_pShaderProgram->SetUniform("tex1", 0);
    m_pCube->Draw();
    //m_pSphere->Draw();
    
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