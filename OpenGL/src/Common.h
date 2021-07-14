#pragma once

struct WindowSize
{
    WindowSize(uint32_t w, uint32_t h) : Width(w), Height(h){}
    
    uint32_t Width, Height;
};

static WindowSize CommonWindowSize[] = {{1366, 768}, {1440, 900}, {1680, 1050}};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


glm::mat3 ComputeNormalMatrix(const glm::mat4 &modelViewMatrix)
{
    return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
}