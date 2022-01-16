#pragma once

namespace Ciao
{
    class GLBuffer
    {
    public:
        GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags);
        ~GLBuffer();

        GLuint getHandle() const { return handle_; }

    private:
        GLuint handle_;
    };
}
