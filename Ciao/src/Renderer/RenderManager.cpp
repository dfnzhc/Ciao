#include "RenderManager.h"

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
        // Initialize OpenGL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        SetClearColour(glm::vec4{0.2f, 0.3f, 0.3f, 1.0f});
    }

    void RenderManager::Shutdown()
    {
        while (m_renderCommands.size() > 0)
        {
            m_renderCommands.pop();
        }
    }

    void RenderManager::Clear()
    {
        while (m_renderCommands.size() > 0)
        {
            m_renderCommands.pop();
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void RenderManager::SetClearColour(const glm::vec4 color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderManager::Submit(std::unique_ptr<RenderCommand> rc)
    {
        m_renderCommands.push(std::move(rc));
    }

    void RenderManager::Flush()
    {
        while (m_renderCommands.size() > 0)
        {
            auto rc = std::move(m_renderCommands.front());
            m_renderCommands.pop();

            rc->Draw();
        }
    }
}
