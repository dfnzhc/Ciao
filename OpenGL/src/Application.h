#pragma once

struct GLFWwindow;

class App
{
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


