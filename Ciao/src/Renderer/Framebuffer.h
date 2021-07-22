#pragma once

#include <glm.hpp>
#include <glad/glad.h>

namespace Ciao
{
    // class Framebuffer
    // {
    // public:
    //     Framebuffer();
    //     ~Framebuffer();
    //
    //     // Create a framebuffer object with a texture of a given size
    //     bool Create(int width, int height);
    //
    //     // Bind the FBO for rendering to texture
    //     void Bind(bool setFullViewport = true);
    //
    //     // Bind the FBO for rendering to texture
    //     void UnBind();
    //
    //     // Bind the texture (usually on a 2nd or later pass in a multi-pass rendering technique)
    //     void BindTexture(int textureUnit);
    //
    //     // Bind the depth (usually on a 2nd or later pass in a multi-pass rendering technique)
    //     void BindDepth(int textureUnit);
    //
    //     // Delete the framebuffer
    //     void Release();
    //
    //     // Set methods for the sampler object
    //     void SetSamplerObjectParameter(GLenum parameter, GLenum value);
    //     void SetSamplerObjectParameterf(GLenum parameter, float value);
    //
    //     // Get the width, height
    //     int GetWidth();
    //     int GetHeight();
    //
    //     inline UINT GetColorTexture() const { return m_ColourTexture; }
    //     inline UINT GetDepthTexture() const { return m_RenderBuffer; }
    //
    // private:
    //     UINT m_FrameBuffer;
    //     UINT m_ColourTexture;
    //     UINT m_RenderBuffer;
    //     UINT m_Sampler;
    //
    //     glm::ivec2 m_Size;
    // };
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height);
        ~Framebuffer();

        inline uint32_t GetFbo() const { return mFbo; }
        inline uint32_t GetTextureId() const { return mTextureId; }
        inline uint32_t GetRenderbufferId() const { return mRenderbufferId; }
        inline const glm::ivec2& GetSize() { return mSize; }
        inline void SetClearColour(const glm::vec4& cc) { mClearColour = cc; }
        inline const glm::vec4& GetClearColour() { return mClearColour; }

    private:
        uint32_t mFbo;
        uint32_t mTextureId;
        uint32_t mRenderbufferId;

        glm::ivec2 mSize;
        glm::vec4 mClearColour;
    };
    
}
