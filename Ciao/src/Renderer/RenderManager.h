#pragma once

#include <queue>
#include "RenderCommand.h"

#define CIAO_SUBMIT_RC(type, ...) std::move(std::make_unique<type>(__VA_ARGS__))

namespace Ciao
{
    class RenderManager
    {
        friend class PushFramebuffer;
        friend class PopFramebuffer;
    public:
        RenderManager();
        ~RenderManager();

        void Init();
        void Shutdown();  

        void Clear();
        void SetClearColour(const glm::vec4 color);
        void Submit(std::shared_ptr<RenderCommand> rc);
        void Flush();

    private:
        void PushFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
        void PopFramebuffer();

    private:
        glm::vec4 m_clearColor;
        std::queue<std::shared_ptr<RenderCommand>> m_renderCommands;
        std::shared_ptr<Framebuffer> m_Framebuffer;
    };
}

