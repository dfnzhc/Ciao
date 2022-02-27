#pragma once

namespace Ciao
{
    constexpr GLuint kBufferIndex_PerFrameUniforms = 0;
    constexpr GLuint kBufferIndex_ModelMatrices = 1;
    constexpr GLuint kBufferIndex_Materials = 2;
    
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
