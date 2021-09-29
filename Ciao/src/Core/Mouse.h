#pragma once

namespace Ciao
{
	enum
	{
		CIAO_INPUT_MOUSE_FIRST		= 0,
		CIAO_INPUT_MOUSE_LEFT		= CIAO_INPUT_MOUSE_FIRST,
		CIAO_INPUT_MOUSE_RIGHT		= 1,
		CIAO_INPUT_MOUSE_MIDDLE		= 2,
		CIAO_INPUT_MOUSE_LAST		= CIAO_INPUT_MOUSE_MIDDLE
	};

	class Mouse
	{
    public:
        static void Init();
        static void Update(GLFWwindow* window);

        inline static int X() { return x; }
        inline static int Y() { return x; }

        inline static int DX() { return x - xLast; }
        inline static int DY() { return y - yLast; }

        static bool Button(int button);
        static bool ButtonDown(int button);
        static bool ButtonUp(int button);
        static bool ButtonStay(int button);

        static void SetButtonState(int button, bool st);
        
    private:
        constexpr static const int ButtonCount = 3;

        static int x, xLast;
        static int y, yLast;

        static std::array<bool, ButtonCount> buttons;
        static std::array<bool, ButtonCount> buttonsLast;
	};
	
}
