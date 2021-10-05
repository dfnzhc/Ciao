#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>


namespace Ciao
{
	class ImGuiRenderer;
	class Mouse;

	class App
	{
	public:
		App();
		~App();

		void swapBuffers();

		bool beginRender();

		GLFWwindow* getWindow() const { return window_; }

		int getWindth() const { return width_; }
		int getHeight() const { return height_; }

		float getDeltaSeconds() const { return  deltaSeconds_; }

		void setTitle(const std::string& title) { title_ = title; }
		inline float getAspect() const { return float(width_) / float(height_); }

	private:
		void setCallback();


		GLFWwindow* window_ = nullptr;

		Mouse* mouse_;

		ImGuiRenderer* imgui_renderer_;

		double timeStamp_ = glfwGetTime();
		float deltaSeconds_ = 0;

		int width_, height_;

		std::string title_;
	};

	void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
}
