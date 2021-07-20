#include "pch.h"
#include "Framebuffer.h"

namespace Ciao
{
    Framebuffer::Framebuffer()
        : m_FrameBuffer(0), m_ColourTexture(0), m_DepthTexture(0), m_Size(glm::vec3{0})
    {
    }

    Framebuffer::~Framebuffer()
    {
        Release();
    }

    bool Framebuffer::Create(int width, int height)
    {
        if (m_FrameBuffer != 0) return false;

        // Create a framebuffer object and bind it
        glGenFramebuffers(1, &m_FrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

        // Create a texture for our colour buffer
        glGenTextures(1, &m_ColourTexture);
        glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
        // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, a_iWidth, a_iHeight); // The Superbible suggests this, but it is OpenGL4.2 feature
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Create a sampler object and set texture properties.  Note here, we're mipmapping
        glGenSamplers(1, &m_Sampler);
        SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Now, create a depth texture for the FBO
        glGenTextures(1, &m_DepthTexture);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, a_iWidth, a_iHeight);  // The Superbible suggests this, but it is OpenGL4.2 feature
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
                     NULL);

        // Now attach the colour and depth textures to the FBO
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColourTexture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthTexture, 0);

        // Tell OpenGL that we want to draw into the frambuffer's colour attachment
        static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, draw_buffers);

        m_Size.x = width;
        m_Size.y = height;

        // Check completeness
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void Framebuffer::Bind(bool setFullViewport)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
        if (setFullViewport)
            glViewport(0, 0, m_Size.x, m_Size.y);

        glm::vec4 clearColour = glm::vec4(0.2, 0.3, 0.7, 1.0);
        float one = 1.0f;
        glClearBufferfv(GL_COLOR, 0, &clearColour.r);
        glClearBufferfv(GL_DEPTH, 0, &one);
    }

    void Framebuffer::BindTexture(int textureUnit)
    {
        glActiveTexture(GL_TEXTURE0+textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
        glBindSampler(textureUnit, m_Sampler);
	
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Framebuffer::BindDepth(int textureUnit)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
        glBindSampler(textureUnit, m_Sampler);
    }

    void Framebuffer::Release()
    {
        if(m_FrameBuffer)
        {
            glDeleteFramebuffers(1, &m_FrameBuffer);
            m_FrameBuffer = 0;
        }
	
        glDeleteSamplers(1, &m_Sampler);
        glDeleteTextures(1, &m_ColourTexture);
        glDeleteTextures(1, &m_DepthTexture);
    }

    void Framebuffer::SetSamplerObjectParameter(GLenum parameter, GLenum value)
    {
        glSamplerParameteri(m_Sampler, parameter, value);
    }

    void Framebuffer::SetSamplerObjectParameterf(GLenum parameter, float value)
    {
        glSamplerParameterf(m_Sampler, parameter, value);
    }

    int Framebuffer::GetWidth()
    {
        return m_Size.x;
    }

    int Framebuffer::GetHeight()
    {
        return m_Size.y;
    }
}
