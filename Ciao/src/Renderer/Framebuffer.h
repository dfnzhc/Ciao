#pragma once

#include <glm.hpp>
#include <glad/glad.h>

namespace Ciao
{
    class Framebuffer
    {
    public:
        Framebuffer();
        ~Framebuffer();

        // Create a framebuffer object with a texture of a given size
        bool Create(int width, int height);

        // Bind the FBO for rendering to texture
        void Bind(bool setFullViewport = true);

        // Bind the texture (usually on a 2nd or later pass in a multi-pass rendering technique)
        void BindTexture(int textureUnit);

        // Bind the depth (usually on a 2nd or later pass in a multi-pass rendering technique)
        void BindDepth(int textureUnit);

        // Delete the framebuffer
        void Release();

        // Set methods for the sampler object
        void SetSamplerObjectParameter(GLenum parameter, GLenum value);
        void SetSamplerObjectParameterf(GLenum parameter, float value);

        // Get the width, height
        int GetWidth();
        int GetHeight();

    private:
        UINT m_FrameBuffer;
        UINT m_ColourTexture;
        UINT m_DepthTexture;
        UINT m_Sampler;

        glm::ivec2 m_Size;
    };
}
