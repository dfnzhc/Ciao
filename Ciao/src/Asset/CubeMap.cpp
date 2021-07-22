#include "pch.h"
#include "CubeMap.h"
#include "FreeImage.h"

namespace Ciao
{
    CubeMap::CubeMap() : m_Texture(0), m_Sampler(0)
    {
    }

    CubeMap::~CubeMap()
    {
        Release();
    }

    void CubeMap::Create(const std::vector<std::string>& filenames)
    {
        int Width, Height;

        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);


        // Load the six sides
        BYTE *pbImagePosX, *pbImageNegX, *pbImagePosY, *pbImageNegY, *pbImagePosZ, *pbImageNegZ;

        LoadTexture(filenames[0], &pbImagePosX, Width, Height);
        LoadTexture(filenames[1], &pbImageNegX, Width, Height);
        LoadTexture(filenames[2], &pbImagePosY, Width, Height);
        LoadTexture(filenames[3], &pbImageNegY, Width, Height);
        LoadTexture(filenames[4], &pbImagePosZ, Width, Height);
        LoadTexture(filenames[5], &pbImageNegZ, Width, Height);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImagePosX);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImageNegX);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImagePosY);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImageNegY);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImagePosZ);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE,
                     pbImageNegZ);

        delete[] pbImagePosX;
        delete[] pbImageNegX;
        delete[] pbImagePosY;
        delete[] pbImageNegY;
        delete[] pbImagePosZ;
        delete[] pbImageNegZ;

        glGenSamplers(1, &m_Sampler);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(m_Sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    void CubeMap::Bind(int texUnit)
    {
        glActiveTexture(GL_TEXTURE0 + texUnit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
        glBindSampler(texUnit, m_Sampler);
    }

    void CubeMap::Release()
    {
        glDeleteSamplers(1, &m_Sampler);
        glDeleteTextures(1, &m_Texture);
    }

    bool CubeMap::LoadTexture(const std::string& filename, BYTE** bmpBytes, int& width, int& height)
    {
        FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
        FIBITMAP* dib(0);

        fif = FreeImage_GetFileType(filename.c_str(), 0); // Check the file signature and deduce its format

        if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
            fif = FreeImage_GetFIFFromFilename(filename.c_str());

        if (fif == FIF_UNKNOWN) // If still unknown, return failure
            return false;

        if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
            dib = FreeImage_Load(fif, filename.c_str());

        if (!dib) {
            CIAO_CORE_ERROR("Cannot load image\n{}\n", filename.c_str());
            return false;
        }

        width = FreeImage_GetWidth(dib);
        height = FreeImage_GetWidth(dib);
        int bpp = FreeImage_GetBPP(dib);

        BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

        // If somehow one of these failed (they shouldn't), return failure
        if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
            return false;
        int test = FreeImage_GetDIBSize(dib);
        *bmpBytes = new BYTE [width * height * bpp / 8];
        memcpy(*bmpBytes, bDataPointer, width * height * bpp / 8);

        FreeImage_Unload(dib);
        return true; // Success
    }
}
