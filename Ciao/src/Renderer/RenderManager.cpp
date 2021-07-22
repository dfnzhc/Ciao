#include "pch.h"
#include "RenderManager.h"

#include "Application.h"
#include "Framebuffer.h"
#include "Window.h"

namespace Ciao
{
    RenderManager::RenderManager()
    {
    }

    RenderManager::~RenderManager()
    {
    }

    void RenderManager::Init()
    {
        CIAO_CORE_INFO("OpenGL Info:\n  Vendor:\t{}\n  Device:\t{}\n  Version:\t{}",
                       glGetString(GL_VENDOR),
                       glGetString(GL_RENDERER),
                       glGetString(GL_VERSION));

        // Initialize OpenGL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        SetClearColour(glm::vec4{0.2f, 0.3f, 0.3f, 1.0f});
    }

    void RenderManager::Shutdown()
    {
        while (m_renderCommands.size() > 0) {
            m_renderCommands.pop();
        }
    }

    void RenderManager::Clear()
    {
        while (m_renderCommands.size() > 0) {
            m_renderCommands.pop();
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderManager::SetClearColour(const glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderManager::Submit(std::shared_ptr<RenderCommand> rc)
    {
        m_renderCommands.push(rc);
    }

    void RenderManager::Flush()
    {
        while (m_renderCommands.size() > 0) {
            auto rc = m_renderCommands.front();
            m_renderCommands.pop();

            rc->Execute();
        }
    }

    void RenderManager::PushFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
    {
        m_Framebuffer = framebuffer;
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetFbo());
        glViewport(0, 0, framebuffer->GetSize().x, framebuffer->GetSize().y);

        auto cc = framebuffer->GetClearColour();
        glClearColor(cc.r, cc.g, cc.b, cc.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderManager::PopFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto window = Application::GetInst().GetWindow();
        glViewport(0, 0, window->GetWidth(), window->GetHeight());
    }
}
