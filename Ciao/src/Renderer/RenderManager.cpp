#include "pch.h"
#include "RenderManager.h"

#include "Application.h"
#include "Framebuffer.h"
#include "Window.h"
#include "glm/gtc/type_ptr.hpp"

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
        // while (m_renderCommands.size() > 0) {
        //     m_renderCommands.pop();
        // }
        //
        //
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto window = Application::GetInst().GetWindow();
        glViewport(0, 0, window->GetWidth(), window->GetHeight());
        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderManager::SetClearColour(const glm::vec4 color)
    {
        m_clearColor = color;
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
        m_Framebuffer->bind();
        
        auto cc = framebuffer->GetClearColour();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
        glClearNamedFramebufferfv(m_Framebuffer->getHandle(), GL_COLOR, 0, glm::value_ptr(glm::vec4(cc.r, cc.g, cc.b, cc.a)));
        glClearNamedFramebufferfi(m_Framebuffer->getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);
        // glClearColor(cc.r, cc.g, cc.b, cc.a);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderManager::PopFramebuffer()
    {
        m_Framebuffer->unbind();
        
        auto window = Application::GetInst().GetWindow();
        glViewport(0, 0, window->GetWidth(), window->GetHeight());
        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}
