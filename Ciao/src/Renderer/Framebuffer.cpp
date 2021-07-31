#include "pch.h"
#include "Framebuffer.h"

#include "Asset/Texture.h"

#include "Application.h"
#include "RenderManager.h"

namespace Ciao
{
    Framebuffer::Framebuffer(uint32_t width, uint32_t height, GLenum formatColor, GLenum formatDepth)
        : mWidth(width), mHeight(height)
    {
        glCreateFramebuffers(1, &m_Handle);

        if (formatColor) {
            m_texColor = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatColor);
            glTextureParameteri(m_texColor->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_texColor->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glNamedFramebufferTexture(m_Handle, GL_COLOR_ATTACHMENT0, m_texColor->getHandle(), 0);
        }

        if (formatDepth) {
            m_texDepth = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatDepth);
            const GLfloat border[] = {0.0f, 0.0f, 0.0f, 0.0f};
            glTextureParameterfv(m_texDepth->getHandle(), GL_TEXTURE_BORDER_COLOR, border);
            glTextureParameteri(m_texDepth->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(m_texDepth->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glNamedFramebufferTexture(m_Handle, GL_DEPTH_ATTACHMENT, m_texDepth->getHandle(), 0);
        }

        const GLenum status = glCheckNamedFramebufferStatus(m_Handle, GL_FRAMEBUFFER);

        CIAO_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer check error!");
    }

    Framebuffer::~Framebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_Handle);
    }

    void Framebuffer::bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
        glViewport(0, 0, mWidth, mHeight);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
        glClearNamedFramebufferfv(m_Handle, GL_COLOR, 0, glm::value_ptr(mClearColour));
        glClearNamedFramebufferfi(m_Handle, GL_DEPTH_STENCIL, 0, 1.0f, 0);
    }

    void Framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        Ciao::Application::GetInst().GetRenderManager()->Clear();
    }
}
