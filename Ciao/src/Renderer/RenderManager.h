#pragma once

#include "Core/Common.h"
#include "RenderCommand.h"

#define CIAO_SUBMIT_RC(type, ...) std::move(std::make_unique<type>(__VA_ARGS__))

namespace Ciao
{
    class RenderManager
    {
    public:
        RenderManager();
        ~RenderManager();

        void Init();
        void Shutdown();

        void Clear();
        void SetClearColour(const glm::vec4 color);
        void Submit(std::unique_ptr<RenderCommand> rc);
        void Flush();
    private:
        std::queue<std::shared_ptr<RenderCommand>> m_renderCommands;
    };
}

