#pragma once
#include "Renderer/GLShader.h"

#include <imgui.h>

namespace Ciao
{
	class ImGuiRenderer
	{
	public:
		ImGuiRenderer() = default;
		~ImGuiRenderer();

		void init(GLFWwindow* window);
		void render();
		void BeginRender();

		bool CaptureMouse();
	};

	void imguiTextureWindowGL(const char* title, uint32_t texId);
}
