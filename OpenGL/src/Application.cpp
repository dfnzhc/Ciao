#include "pch.h"
#include "Application.h"
#include "Common.h"
#include "Shader.h"
#include "Cube.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "Camera.h"
#include "OpenAssetImportMesh.h"

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
    m_wWidth = CommonWindowSize[1].Width;
    m_wHeight = CommonWindowSize[1].Height;
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
    m_MouseEnable = false;
    m_VAO = 0;

    /// 场景物体
    m_pCube = nullptr;
    m_pSphere = nullptr;
    m_pCamera = nullptr;
    m_pTest = nullptr;
}

App::~App()
{
    delete m_pShaderProgram;
    delete m_pCube;
    delete m_pSphere;
    delete m_pCamera;
    delete m_pTest;
}

void App::Init()
{
    /// 创建物体
    m_pCube = new Cube;
    m_pSphere = new Sphere;
    m_pTest = new OpenAssetImportMesh;

    // 初始化摄像机
    m_pCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    m_pCamera->SetProjectionMatrix((float)m_wWidth / (float)m_wHeight, 0.1f, 1000.0f);
    
    // 加载 Shader 文件
    std::vector<Shader> Shaders;
    std::vector<std::string> ShaderFileNames;
    ShaderFileNames.push_back("shader.vert");
    ShaderFileNames.push_back("shader.frag");

    ReadShaderFile(ShaderFileNames, Shaders);

    // 创建 OpenGL Shader 程序
    m_pShaderProgram = new ShaderProgram;
    m_pShaderProgram->CreateProgram();
    m_pShaderProgram->AddShaderToProgram(&Shaders[0]);
    m_pShaderProgram->AddShaderToProgram(&Shaders[1]);
    m_pShaderProgram->LinkProgram();

    m_pCube->Create("resources\\Textures\\container.jpg");
    m_pSphere->Create("resources\\Textures\\container.jpg", 25, 25);

    //m_pTest->Load("resources\\Models\\b\\source\\Cyborg_ninja_sketchfab.fbx");
    m_pTest->Load("resources\\Models\\House\\House.obj");
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
        ImGui::Text("FPS %.1f FPS (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

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

    /// 创建矩阵工具栈
    glutil::MatrixStack modelViewMatrixStack;
    modelViewMatrixStack.SetMatrix(m_pCamera->GetViewMatrix());

    m_pShaderProgram->UseProgram();
    m_pShaderProgram->SetUniform("tex1", 0);
    
    m_pShaderProgram->SetUniform("projMatrix", m_pCamera->GetProjectionMatrix());


    /// 立方体的渲染
    modelViewMatrixStack.Push();
        modelViewMatrixStack.Translate(glm::vec3(0.0f, -0.5f, 0.0f));
        modelViewMatrixStack.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float)glfwGetTime());
        modelViewMatrixStack.Scale(glm::vec3(0.2f));
        m_pShaderProgram->SetUniform("modelViewMatrix", modelViewMatrixStack.Top());
        m_pShaderProgram->SetUniform("normalMatrix", ComputeNormalMatrix(modelViewMatrixStack.Top()));
        m_pTest->Draw();
    modelViewMatrixStack.Pop();
    
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::ProcessInput()
{
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pGlfwWindow, true);

    /// 相机位置的操纵
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_W) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(FORWARD, m_dt);
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_S) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(BACKWARD, m_dt);
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_A) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(LEFT, m_dt);
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_D) == GLFW_PRESS)
        m_pCamera->ProcessKeyboard(RIGHT, m_dt);

    /// 鼠标的捕获事件
    if (glfwGetKey(m_pGlfwWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        if (m_MouseEnable) {
            m_MouseEnable = false;
            glfwSetInputMode(m_pGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            m_MouseEnable = true;
            glfwSetInputMode(m_pGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void App::Execute()
{
    Init();

    while (m_appState == AppState::RUNNING) {
        double t = glfwGetTime();
        m_dt = t - m_elapsedTime;
        m_elapsedTime = t;

        ProcessInput();
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
    glfwSetWindowPos(m_pGlfwWindow, 0, 0);
    glfwMakeContextCurrent(m_pGlfwWindow);
    if (m_pGlfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Error");
    }

    // Set the required callback functions
    glfwSetFramebufferSizeCallback(m_pGlfwWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_pGlfwWindow, mouse_callback);
    glfwSetScrollCallback(m_pGlfwWindow, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_pGlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetKeyCallback(m_pGlfwWindow, key_callback);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (GameInst->m_pCamera->IsFirstSetMouse())
    {
        GameInst->m_pCamera->SetMousePos(xpos, ypos);
        GameInst->m_pCamera->SetMouseMode();
    }

    auto [mouseX, mouseY] = GameInst->m_pCamera->GetMousePos();
    float xoffset = xpos - mouseX;
    float yoffset = mouseY - ypos;

    GameInst->m_pCamera->SetMousePos(xpos, ypos);
    
    if (!GameInst->m_MouseEnable)
        GameInst->m_pCamera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameInst->m_pCamera->ProcessMouseScroll(yoffset);
}