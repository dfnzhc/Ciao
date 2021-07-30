#include "pch.h"
#include "Framebuffer.h"

namespace Ciao
{
    // Framebuffer::Framebuffer()
    //     : m_FrameBuffer(0), m_ColourTexture(0), m_RenderBuffer(0), m_Size(glm::vec3{0})
    // {
    // }
    //
    // Framebuffer::~Framebuffer()
    // {
    //     Release();
    // }
    //
    // bool Framebuffer::Create(int width, int height)
    // {
    //     if (m_FrameBuffer != 0) return false;
    //
    //     // Create a framebuffer object and bind it
    //     glGenFramebuffers(1, &m_FrameBuffer);
    //     glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
    //
    //     // Create a texture for our colour buffer
    //     glGenTextures(1, &m_ColourTexture);
    //     glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
    //     // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, a_iWidth, a_iHeight); // The Superbible suggests this, but it is OpenGL4.2 feature
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    //
    //     // Create a sampler object and set texture properties.  Note here, we're mipmapping
    //     glGenSamplers(1, &m_Sampler);
    //     SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //
    //     // Now, create a depth texture for the FBO
    //     glGenTextures(1, &m_RenderBuffer);
    //     glBindTexture(GL_TEXTURE_2D, m_RenderBuffer);
    //     // glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, a_iWidth, a_iHeight);  // The Superbible suggests this, but it is OpenGL4.2 feature
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE,
    //                  NULL);
    //
    //     // Now attach the colour and depth textures to the FBO
    //     glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColourTexture, 0);
    //     glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_RenderBuffer, 0);
    //
    //     // Tell OpenGL that we want to draw into the frambuffer's colour attachment
    //     static const GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0};
    //     glDrawBuffers(1, draw_buffers);
    //
    //     m_Size.x = width;
    //     m_Size.y = height;
    //
    //     // Check completeness
    //     return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    // }
    //
    // void Framebuffer::Bind(bool setFullViewport)
    // {
    //     glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
    //     if (setFullViewport)
    //         glViewport(0, 0, m_Size.x, m_Size.y);
    //
    //     glm::vec4 clearColour = glm::vec4(0.1, 0.1, 0.2, 1.0);
    //     float one = 1.0f;
    //     glClearBufferfv(GL_COLOR, 0, &clearColour.r);
    //     glClearBufferfv(GL_DEPTH, 0, &one);
    //     // glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
    //     //
    //     // // make sure we clear the framebuffer's content
    //     // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    //     // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // }
    //
    // void Framebuffer::UnBind()
    // {
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //     glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    //     glClear(GL_COLOR_BUFFER_BIT);
    // }
    //
    // void Framebuffer::BindTexture(int textureUnit)
    // {
    //     glActiveTexture(GL_TEXTURE0+textureUnit);
    //     glBindTexture(GL_TEXTURE_2D, m_ColourTexture);
    //     glBindSampler(textureUnit, m_Sampler);
	   //
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    //
    // void Framebuffer::BindDepth(int textureUnit)
    // {
    //     glActiveTexture(GL_TEXTURE0 + textureUnit);
    //     glBindTexture(GL_TEXTURE_2D, m_RenderBuffer);
    //     glBindSampler(textureUnit, m_Sampler);
    // }
    //
    // void Framebuffer::Release()
    // {
    //     if(m_FrameBuffer)
    //     {
    //         glDeleteFramebuffers(1, &m_FrameBuffer);
    //         m_FrameBuffer = 0;
    //     }
	   //
    //     glDeleteSamplers(1, &m_Sampler);
    //     glDeleteTextures(1, &m_ColourTexture);
    //     glDeleteTextures(1, &m_RenderBuffer);
    // }
    //
    // void Framebuffer::SetSamplerObjectParameter(GLenum parameter, GLenum value)
    // {
    //     glSamplerParameteri(m_Sampler, parameter, value);
    // }
    //
    // void Framebuffer::SetSamplerObjectParameterf(GLenum parameter, float value)
    // {
    //     glSamplerParameterf(m_Sampler, parameter, value);
    // }
    //
    // int Framebuffer::GetWidth()
    // {
    //     return m_Size.x;
    // }
    //
    // int Framebuffer::GetHeight()
    // {
    //     return m_Size.y;
    // }
	
	Framebuffer::Framebuffer(uint32_t width, uint32_t height)
    		: mFbo(0)
    		, mTextureId(0)
    		, mRenderbufferId(0)
    		, mSize({ width, height })
    		, mClearColour(1.f)
    	{
    		glGenFramebuffers(1, &mFbo); 
    		glBindFramebuffer(GL_FRAMEBUFFER, mFbo); 
    
    		// Create colour texture
    		glGenTextures(1, &mTextureId); 
    		glBindTexture(GL_TEXTURE_2D, mTextureId); 
    		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mSize.x, mSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); 
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    		glBindTexture(GL_TEXTURE_2D, 0); 
    		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, 0);

            glGenTextures(1, &mDepthMap);
            glBindTexture(GL_TEXTURE_2D, mDepthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mSize.x, mSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
			// glDrawBuffer(GL_NONE);
			// glReadBuffer(GL_NONE);
    
    		// // Create depth/stencil renderbuffer
    		// glGenRenderbuffers(1, &mRenderbufferId); 
    		// glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferId); 
    		// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mSize.x, mSize.y); 
    		// glBindRenderbuffer(GL_RENDERBUFFER, 0);
    		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferId); 
    
    		// Check for completeness
    		int32_t completeStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER); 
    		if (completeStatus != GL_FRAMEBUFFER_COMPLETE)
    		{
    			CIAO_CORE_ERROR("Failure to create framebuffer. Complete status: {}", completeStatus);
    		}
    
    		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    	}
    
    	Framebuffer::~Framebuffer()
    	{
    		glDeleteFramebuffers(1, &mFbo);
    		mFbo = 0;
    		mTextureId = 0;
    		mRenderbufferId = 0;
    	}
}
