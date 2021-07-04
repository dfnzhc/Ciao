#pragma once

struct WindowSize
{
    WindowSize(uint32_t w, uint32_t h) : Width(w), Height(h){}
    
    uint32_t Width, Height;
};

static WindowSize CommonWindowSize[] = {{1366, 768}, {1440, 900}, {1680, 1050}};

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}