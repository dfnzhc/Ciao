#include "pch.h"
#include "Mouse.h"

#include "Application.h"
#include "Window.h"

namespace Ciao
{
    int Mouse::x            = 0;
    int Mouse::y            = 0;
    int Mouse::xLast        = 0;
    int Mouse::yLast        = 0;

    std::array<bool, Mouse::ButtonCount> Mouse::buttons;
    std::array<bool, Mouse::ButtonCount> Mouse::buttonsLast;
    
    void Mouse::Init()
    {
        std::fill(buttons.begin(), buttons.end(), false);
        std::fill(buttonsLast.begin(), buttonsLast.end(), false);
    }

    void Mouse::Update()
    {
        xLast = x;
        yLast = y;
        buttonsLast = buttons;

        double xpos, ypos;
        glfwGetCursorPos(Application::GetInst().GetWindow()->GetWindow(), &xpos, &ypos);

        x = static_cast<int>(xpos);
        y = static_cast<int>(ypos);

        for (int i = 0; i < ButtonCount; ++i) {
            buttons[i] = glfwGetMouseButton(Application::GetInst().GetWindow()->GetWindow(), i) == GLFW_PRESS;
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

    void Mouse::SetButtonState(int button, bool st)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            buttonsLast[button] = buttons[button];
            buttons[button] = st;
        }
    }

    void Mouse::UpdatePos(int nx, int ny)
    {
        xLast = x;
        yLast = y;

        x = nx;
        y = ny;
    }
}
