#pragma once

#include <glm.hpp>

namespace Ciao
{
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height);
        ~Framebuffer();

        inline uint32_t GetFbo() const { return m_Fbo; }
        inline uint32_t GetTextureId() const { return m_Texture; }
        inline uint32_t GetRenderbufferId() const { return m_Renderbuffer; }
        inline const glm::ivec2& GetSize() { return m_Size; }
        inline void SetClearColour(const glm::vec4& cc) { m_ClearColour = cc; }
        inline const glm::vec4& GetClearColour() { return m_ClearColour; }

    private:
        uint32_t m_Fbo;
        uint32_t m_Texture;
        uint32_t m_Renderbuffer;

        glm::ivec2 m_Size;
        glm::vec4 m_ClearColour;
    };
}


