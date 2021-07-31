#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Ciao
{
    class Texture;
    
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height, GLenum formatColor = GL_RGBA8, GLenum formatDepth = GL_DEPTH_COMPONENT24);
        ~Framebuffer();
        Framebuffer(const Framebuffer&);
        Framebuffer(Framebuffer&&);

        inline GLuint getHandle() const { return m_Handle; }
        inline const Texture& getTextureColor() const { return *m_texColor.get(); }
        inline const Texture& getTextureDepth() const { return *m_texDepth.get(); }
        
        void bind();
        void unbind();
        
        inline void SetClearColour(const glm::vec4& cc) { mClearColour = cc; }
        inline const glm::vec4& GetClearColour() { return mClearColour; }

    private:
        uint32_t mWidth, mHeight;
        GLuint m_Handle = 0;
        
        glm::vec4 mClearColour;

        std::unique_ptr<Texture> m_texColor;
        std::unique_ptr<Texture> m_texDepth;
    };
    
}
