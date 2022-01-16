#include "pch.h"
#include "Mouse.h"

namespace Ciao
{
    std::array<bool, Mouse::ButtonCount> Mouse::buttons;
    std::array<bool, Mouse::ButtonCount> Mouse::buttonsLast;

    void Mouse::Init()
    {
        std::fill(buttons.begin(), buttons.end(), false);
        std::fill(buttonsLast.begin(), buttonsLast.end(), false);
    }

    void Mouse::Update(GLFWwindow* window)
    {
        xLast = x;
        yLast = y;
        buttonsLast = buttons;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        x = static_cast<int>(xpos);
        y = static_cast<int>(ypos);

        for (int i = 0; i < ButtonCount; ++i)
        {
            buttons[i] = glfwGetMouseButton(window, i) == GLFW_PRESS;
        }
    }

    bool Mouse::Button(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button];
        }
        return false;
    }

    bool Mouse::ButtonDown(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button] && !buttonsLast[button];
        }
        return false;
    }

    bool Mouse::ButtonUp(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return !buttons[button] && buttonsLast[button];
        }
        return false;
    }

    bool Mouse::ButtonStay(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button] && buttonsLast[button];
        }
        return false;
    }
}
