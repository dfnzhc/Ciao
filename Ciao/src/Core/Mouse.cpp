#include "pch.h"
#include "Mouse.h"

#include <SDL_events.h>
#include <SDL_mouse.h>
#include <SDL_stdinc.h>

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
        Uint32 state = SDL_GetMouseState(&x, &y);
        for (int i = 0; i < ButtonCount; ++i) {
            buttons[i] = state & SDL_BUTTON(i + 1);
        }
    }

    bool Mouse::Button(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button - 1];
        }
        return false;
    }

    bool Mouse::ButtonDown(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button - 1] && !buttonsLast[button - 1];
        }
        return false;
    }

    bool Mouse::ButtonUp(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return !buttons[button - 1] && buttonsLast[button - 1];
        }
        return false;
    }

    bool Mouse::ButtonStay(int button)
    {
        if (button >= CIAO_INPUT_MOUSE_FIRST && button <= CIAO_INPUT_MOUSE_LAST)
        {
            return buttons[button - 1] && buttonsLast[button - 1];
        }
        return false;
    }
}
