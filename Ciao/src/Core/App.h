#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>


namespace Ciao
{
	class Mouse;

	class App
	{
	public:
		App();
		~App();

		void swapBuffers();

		bool shouldClose() const;

		GLFWwindow* getWindow() const { return window_; }

		uint32_t getWindth() const { return width_; }
		uint32_t getHeight() const { return height_; }

		float getDeltaSeconds() const { return  deltaSeconds_; }

		void setTitle(const std::string& title) { title_ = title; }

	private:
		void setCallback();


		GLFWwindow* window_ = nullptr;

		Mouse* mouse_;

		double timeStamp_ = glfwGetTime();
		float deltaSeconds_ = 0;

		uint32_t width_, height_;

		std::string title_;
	};

	void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
}
