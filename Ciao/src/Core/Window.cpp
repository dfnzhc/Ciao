#include "pch.h"
#include "Window.h"
#include "Application.h"

#include "Mouse.h"
#include "Demo.h"

namespace Ciao
{
	void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

	WindowProps::WindowProps()
		: Title("Ciao Renderer"), XPos(100), YPos(50),
		  Width(1600), Height(980)
	{
	}

	Window::Window() : window_(nullptr), width_(0), height_(0)
	{
		m_ImguiWindow = CreateRef<ImguiWindow>();
	}

	Window::~Window()
	{
		if (window_)
		{
			Shutdown();
		}
	}

	uint32_t Window::GetWidth() const
	{
		return width_;
	}

	uint32_t Window::GetHeight() const
	{
		return height_;
	}

	bool Window::Create(const WindowProps& props)
	{
		glfwSetErrorCallback([](int error, const char* description)
		{
				CIAO_CORE_ERROR("Error: {}.", description);
		});
		
		if (!glfwInit())
			return false;
		
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		
		width_ = props.Width;
		height_ = props.Height;
		
		window_ = glfwCreateWindow(width_, height_, props.Title.c_str(), nullptr, nullptr);
		glfwSetWindowPos(window_, props.XPos, props.YPos);

		if (!window_)
		{
			glfwTerminate();
			CIAO_CORE_ERROR("Create GLFW window FAILED.");
			return false;
		}

		glfwMakeContextCurrent(window_);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			CIAO_CORE_ERROR("Failed to initialize GLAD");
			return false;
		}

		glfwSwapInterval(0);

		glDebugMessageCallback(message_callback, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);


		m_ImguiWindow->Create(props.ImguiProps);

		return true;
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(window_);
		glfwTerminate();
		window_ = nullptr;
	}

	void Window::HadleEvents()
	{
		// SDL_Event e;
		// while (SDL_PollEvent(&e))
		// {
		// 	switch (e.type)
		// 	{
		// 	case SDL_QUIT:
		// 		// 窗口关闭
		// 		Application::GetInst().Shutdown();
		// 		break;
		// 	case SDL_KEYDOWN:
		// 		if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		// 		{
		// 			Application::GetInst().Shutdown();
		// 		}
		// 		break;
		// 	case SDL_MOUSEWHEEL:
		// 		if (e.wheel.y > 0) Camera::UpdateDistance(-0.5);
		// 		if (e.wheel.y < 0) Camera::UpdateDistance(0.5);
		// 		break;
		//
		// 	default:
		// 		break;
		// 	}
		// }

		glfwPollEvents();

		if (!m_ImguiWindow->CaptureMouse())
		{
			Mouse::Update();
		}
	}

	void Window::BeginRender()
	{

	}

	void Window::EndRender()
	{
		m_ImguiWindow->BeginRender();
		Application::GetInst().GetScence()->ImguiRender();
		m_ImguiWindow->EndRender();

		glfwSwapBuffers(window_);
		assert(glGetError() == GL_NO_ERROR);

		const double newTimeStamp = glfwGetTime();
		deltaSeconds_ = static_cast<float>(newTimeStamp - timeStamp_);
		timeStamp_ = newTimeStamp;
	}

	void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
	{
		auto const src_str = [source]() {
			switch (source)
			{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			}
			return "";
		}();

		auto const type_str = [type]() {
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER: return "MARKER";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			}
			return "";
		}();

		auto const severity_str = [severity]() {
			switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			}
			return "";
		}();

		CIAO_CORE_INFO("{}, {}, {}, {}: {}.", src_str, type_str, severity_str, id, message);
	}
}
