#include "pch.h"
#include "Application.h"
#include "Demo.h"
#include "Window.h"


#include "Mouse.h"
#include "Camera.h"


namespace Ciao
{
	Application* Application::m_pAppInst = nullptr;

	Application& Application::GetInst()
	{
		if (m_pAppInst == nullptr)
		{
			m_pAppInst = new Application();
		}
		return *m_pAppInst;
	}

	Application::Application()
		: m_Demo(nullptr), m_Window(nullptr)
	{
		m_Window = CreateRef<Window>();
		m_Camera = CreateRef<Camera>();
	}

	Application::~Application()
	{
	}

	void Application::Execute(Demo* scence)
	{
		m_Demo = std::shared_ptr<Demo>(scence);
		if (Init())
		{
			while (!glfwWindowShouldClose(m_Window->GetWindow()))
			{
				Update();
				Render();
			}

			Terminate();
		}
	}

	void Application::GetWindowSize(uint32_t& w, uint32_t& h)
	{
		if (m_Window)
		{
			w = m_Window->GetWidth();
			h = m_Window->GetHeight();
		}
	}

	bool Application::Init()
	{
		WindowProps props = m_Demo->GetWindowProps();
		if (m_Window->Create(props))
		{
			CIAO_CORE_INFO("Window initializing SUCCESSED..");

			Mouse::Init();
			m_Camera->Init();
			m_Camera->SetProjectionMatrix(45.0f,
			                              (float)m_Window->GetWidth() / (float)m_Window->GetHeight(),
			                              0.1f, 1000.0f);

			m_Demo->Init();

			SetGLFWCallBack();

			return true;
		}


		CIAO_CORE_ERROR("Applicaition initializing FAILED. Terminating..");
		Terminate();

		return false;
	}

	void Application::Terminate()
	{
		m_Demo->Shutdown();
		m_Window->Shutdown();

		// SDL_Quit();
	}

	void Application::Update()
	{
		m_Window->HadleEvents();
		m_Camera->Update();
		m_Demo->Update();
	}

	void Application::Render()
	{
		int width, height;
		glfwGetFramebufferSize(m_Window->GetWindow(), &width, &height);
		const float ratio = width / (float)height;

		glViewport(0, 0, width, height);

		m_Window->BeginRender();
		m_Demo->Render();
		m_Window->EndRender();
	}

	void Application::SetGLFWCallBack()
	{
		// glfwSetCursorPosCallback(
		//     m_Window->GetWindow(),
		//     [](auto* window, double x, double y)
		//     {
		//         Mouse::UpdatePos(x, y);
		//     }
		// );

		// glfwSetMouseButtonCallback(
		//     m_Window->GetWindow(), 
		//     [](GLFWwindow* window, int button, int action, int mods)
		//     {
		//         Mouse::SetButtonState(button, action == GLFW_PRESS);
		//     }
		// );

		glfwSetScrollCallback(
			m_Window->GetWindow(),
			[](GLFWwindow* window, double xoffset, double yoffset)
			{
				if (yoffset > 0)
					Camera::UpdateDistance(-0.5f);
				else if (yoffset < 0)
					Camera::UpdateDistance(0.5f);
			}
		);

		glfwSetKeyCallback(
			m_Window->GetWindow(),
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				const bool pressed = action != GLFW_RELEASE;
				if (key == GLFW_KEY_ESCAPE && pressed)
					glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		);
	}
}
