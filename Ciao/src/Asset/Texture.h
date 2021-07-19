#pragma once

namespace Ciao
{
    class Texture
    {
    public:
        void CreateFromData(BYTE* data, int width, int height, int bpp, GLenum format, bool generateMipMaps = false);
        bool Load(std::string path, bool generateMipMaps = true);
        void Bind(int textureUnit = 0);

        void SetSamplerObjectParameter(GLenum parameter, GLenum value);
        void SetSamplerObjectParameterf(GLenum parameter, float value);

        int GetWidth();
        int GetHeight();
        int GetBPP();

        void Release();

        Texture();
        ~Texture();

    private:
        int m_width, m_height, m_bpp;       // Texture width, height, and bytes per pixel
        GLuint m_textureID;                 // Texture id
        GLuint m_samplerObjectID;           // Sampler id
        bool m_mipMapsGenerated;

        std::string m_path;
    };
}


