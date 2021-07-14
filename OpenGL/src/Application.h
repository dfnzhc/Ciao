#pragma once

/// 使用类的前置声明
struct GLFWwindow;
class Shader;
class ShaderProgram;
class Cube;
class Sphere;
class Camera;

class App
{
    friend void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
private:
    glm::vec4 m_bgColor;
    glm::vec4 m_triColor;
    ShaderProgram* m_pShaderProgram;
    GLuint m_VAO;
    Cube* m_pCube;
    Sphere* m_pSphere;
    Camera* m_pCamera;

private:
    GLFWwindow* m_pGlfwWindow;
    
    uint32_t m_wWidth;
    uint32_t m_wHeight;
    double m_dt;
    double m_elapsedTime;
    bool m_MouseEnable;

    enum class AppState { RUNNING, TERMINATE };
    AppState m_appState;
private:
    void InitGlfwWindow();

    void Init();
    void Update();
    void Render();

    void ProcessInput();
    
public:
    App();
    ~App();

    static App& GetInst();

    void Execute();

};


