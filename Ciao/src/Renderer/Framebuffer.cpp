﻿#include "pch.h"
#include "Framebuffer.h"

#include "Texture.h"

namespace Ciao
{
	Framebuffer::Framebuffer(int width, int height, GLenum formatColor, GLenum formatDepth)
		: width_(width)
		, height_(height)
	{
		glCreateFramebuffers(1, &handle_);

		if (formatColor)
		{
			texColor_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatColor);
			glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(texColor_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0, texColor_->getHandle(), 0);
		}
		if (formatDepth)
		{
			texDepth_ = std::make_unique<Texture>(GL_TEXTURE_2D, width, height, formatDepth);
			const GLfloat border[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glTextureParameterfv(texDepth_->getHandle(), GL_TEXTURE_BORDER_COLOR, border);
			glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTextureParameteri(texDepth_->getHandle(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glNamedFramebufferTexture(handle_, GL_DEPTH_ATTACHMENT, texDepth_->getHandle(), 0);
		}

		const GLenum status = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER);

		assert(status == GL_FRAMEBUFFER_COMPLETE);
	}

	Framebuffer::~Framebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &handle_);
	}

	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, handle_);
		glViewport(0, 0, width_, height_);
	}

	void Framebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
