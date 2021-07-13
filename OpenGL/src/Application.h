#pragma once

/// 使用类的前置声明
struct GLFWwindow;
class Shader;
class ShaderProgram;

class App
{
private:
    glm::vec4 m_bgColor;
    glm::vec4 m_triColor;
    ShaderProgram* m_pShaderProgram;
    GLuint m_VAO;


private:
    GLFWwindow* m_pGlfwWindow;
    
    uint32_t m_wWidth;
    uint32_t m_wHeight;
    double m_dt;
    double m_elapsedTime;


    enum class AppState { RUNNING, TERMINATE };
    AppState m_appState;
private:
    void InitGlfwWindow();

    void Init();
    void Update();
    void Render();
    
public:
    App();
    ~App();

    static App& GetInst();

    void Execute();

};


