#include "pch.h"
#include "Texture.h"
#include "FreeImage.h"


namespace Ciao
{
    Texture::Texture()
    {
        m_mipMapsGenerated = false;
    }

    Texture::~Texture()
    {
        Release();
    }

    /// 用 data 中的数据创建纹理
    void Texture::CreateFromData(BYTE* data, int width, int height, int bpp, GLenum format, bool generateMipMaps)
    {
        // Generate an OpenGL texture ID for this texture
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        if(format == GL_RGBA || format == GL_BGRA)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // We must handle this because of internal format parameter
        else if(format == GL_RGB || format == GL_BGR)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        if(generateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
        glGenSamplers(1, &m_samplerObjectID);

        m_path = "";
        m_mipMapsGenerated = generateMipMaps;
        m_width = width;
        m_height = height;
        m_bpp = bpp;
    }

    /// 从文件加载纹理
    bool Texture::Load(std::string path, bool generateMipMaps)
    {
        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        FIBITMAP* dib(0);

        fif = FreeImage_GetFileType(path.c_str(), 0); // Check the file signature and deduce its format

        if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
            fif = FreeImage_GetFIFFromFilename(path.c_str());
	    
        if(fif == FIF_UNKNOWN) // If still unknown, return failure
            return false;

        if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
            dib = FreeImage_Load(fif, path.c_str());

        if(!dib) {
            char message[1024];
            sprintf_s(message, "Cannot load image\n%s\n", path.c_str());
            return false;
        }

        BYTE* pData = FreeImage_GetBits(dib); // Retrieve the image data

        // If somehow one of these failed (they shouldn't), return failure
        if (pData == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
            return false;
	    

        GLenum format;
        int bada = FreeImage_GetBPP(dib);
        if(FreeImage_GetBPP(dib) == 32)format = GL_BGRA;
        if(FreeImage_GetBPP(dib) == 24)format = GL_BGR;
        if(FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
        CreateFromData(pData, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, generateMipMaps);
	    
        FreeImage_Unload(dib);

        m_path = path;

        return true;            // Success
    }

    void Texture::Bind(int textureUnit)
    {
        glActiveTexture(GL_TEXTURE0+textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glBindSampler(textureUnit, m_samplerObjectID);
    }

    void Texture::SetSamplerObjectParameter(GLenum parameter, GLenum value)
    {
        glSamplerParameteri(m_samplerObjectID, parameter, value);
    }

    void Texture::SetSamplerObjectParameterf(GLenum parameter, float value)
    {
        glSamplerParameterf(m_samplerObjectID, parameter, value);
    }

    int Texture::GetWidth()
    {
        return m_width;
    }

    int Texture::GetHeight()
    {
        return m_height;
    }

    int Texture::GetBPP()
    {
        return m_bpp;
    }

    void Texture::Release()
    {
        glDeleteSamplers(1, &m_samplerObjectID);
        glDeleteTextures(1, &m_textureID);
    }
}