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
        GLenum getType() const { return m_Type; }
        GLuint getHandle() const { return m_Handle; }
        GLuint64 getHandleBindless() const { return m_HandleBindless; }

    private:
        GLenum m_Type = 0;
        GLuint m_Handle = 0;
        GLuint64 m_HandleBindless = 0;
    };
}


