#pragma once

namespace Ciao
{
	class Texture
	{
	public:
		Texture(GLenum type, const char* fileName);
		Texture(GLenum type, int width, int height, GLenum internalFormat);
		Texture(int w, int h, const void* img);
		~Texture();
		Texture(const Texture&) = delete;
		Texture(Texture&&);
		GLenum getType() const { return type_; }
		GLuint getHandle() const { return handle_; }
		GLuint64 getHandleBindless() const { return handleBindless_; }

	private:
		GLenum type_ = 0;
		GLuint handle_ = 0;
		GLuint64 handleBindless_ = 0;
	};
}
