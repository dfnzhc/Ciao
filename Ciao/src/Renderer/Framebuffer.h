#pragma once

namespace Ciao
{
	class Texture;

	class Framebuffer
	{
	public:
		Framebuffer(int width, int height, GLenum formatColor, GLenum formatDepth);
		~Framebuffer();
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = default;
		GLuint getHandle() const { return handle_; }
		const Texture& getTextureColor() const { return *texColor_.get(); }
		const Texture& getTextureDepth() const { return *texDepth_.get(); }
		void bind();
		void unbind();

	private:
		int width_;
		int height_;
		GLuint handle_ = 0;

		std::unique_ptr<Texture> texColor_;
		std::unique_ptr<Texture> texDepth_;
	};
}
