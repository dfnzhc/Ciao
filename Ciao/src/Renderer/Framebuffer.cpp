#include "pch.h"
#include "Framebuffer.h"

namespace Ciao
{
    Framebuffer::Framebuffer(uint32_t width, uint32_t height) 
        : m_Fbo(0), m_Texture(0), m_Renderbuffer(0), m_Size({ width, height }), m_ClearColour(1.f)
    {
        glGenBuffers(1, &m_Fbo);
        glBindBuffer(GL_FRAMEBUFFER, m_Fbo);

        // 创建缓冲区图像
        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Size.x, m_Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

        // 创建 深度/模板 缓冲区
        glGenRenderbuffers(1, &m_Renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Size.x, m_Size.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            CIAO_CORE_ERROR("Creat frame buffer FAILED!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_Fbo);
        m_Fbo = 0;
        m_Texture = 0;
        m_Renderbuffer = 0;
    }
}
