#include "pch.h"
#include "Texture.h"
#include "Utils/Bitmap.h"
#include "Utils/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <gli/gli.hpp>
#include <gli/load_ktx.hpp>

namespace Ciao
{
    Texture::Texture(GLenum type, int width, int height, GLenum internalFormat)
        : m_Type(type)
    {
        glCreateTextures(type, 1, &m_Handle);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(m_Handle, getNumMipMapLevels2D(width, height), internalFormat, width, height);
    }

    Texture::Texture(GLenum type, const char* fileName)
        : m_Type(type)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glCreateTextures(type, 1, &m_Handle);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const char* ext = strrchr(fileName, '.');

        const bool isKTX = ext && !strcmp(ext, ".ktx");
        const bool isHDR = ext && !strcmp(ext, ".hdr");

        switch (type)
        {
        case GL_TEXTURE_2D:
            {
                int w = 0;
                int h = 0;
                int numMipmaps = 0;
                if (isKTX)
                {
                    gli::texture gliTex = gli::load_ktx(fileName);
                    gli::gl GL(gli::gl::PROFILE_KTX);
                    gli::gl::format const format = GL.translate(gliTex.format(), gliTex.swizzles());
                    glm::tvec3<GLsizei> extent(gliTex.extent(0));
                    w = extent.x;
                    h = extent.y;
                    numMipmaps = getNumMipMapLevels2D(w, h);
                    glTextureStorage2D(m_Handle, numMipmaps, format.Internal, w, h);
                    glTextureSubImage2D(m_Handle, 0, 0, 0, w, h, format.External, format.Type, gliTex.data(0, 0, 0));
                }
                else if (isHDR) // TODO
                {
                    const float* img = stbi_loadf(fileName, &w, &h, nullptr, STBI_rgb);
                    CIAO_ASSERT(img, "Image load Faild: " + std::string(fileName));
                    numMipmaps = 1;//getNumMipMapLevels2D(w, h);
                    glTextureStorage2D(m_Handle, numMipmaps, GL_RGB16F, w, h);
                    glTextureSubImage2D(m_Handle, 0, 0, 0, w, h, GL_RGB16F, GL_FLOAT, img);
                    stbi_image_free((void*)img);
                }
                else
                {
                    const uint8_t* img = stbi_load(fileName, &w, &h, nullptr, STBI_rgb_alpha);
                    CIAO_ASSERT(img, "Image load Faild: " + std::string(fileName));
                    numMipmaps = getNumMipMapLevels2D(w, h);
                    glTextureStorage2D(m_Handle, numMipmaps, GL_RGBA8, w, h);
                    glTextureSubImage2D(m_Handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
                    stbi_image_free((void*)img);
                }
                glGenerateTextureMipmap(m_Handle);
                glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
                glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(m_Handle, GL_TEXTURE_MAX_ANISOTROPY, 16);
                break;
            }
        case GL_TEXTURE_CUBE_MAP:
            {
                int w, h, comp;
                const float* img = stbi_loadf(fileName, &w, &h, &comp, 3);
                CIAO_ASSERT(img, "Image load Faild: " + std::string(fileName));
                Bitmap in(w, h, comp, BitmapFormat_Float, img);
                const bool isEquirectangular = w == 2 * h;
                Bitmap out = isEquirectangular ? convertEquirectangularMapToVerticalCross(in) : in;
                stbi_image_free((void*)img);
                Bitmap cubemap = convertVerticalCrossToCubeMapFaces(out);

                const int numMipmaps = getNumMipMapLevels2D(cubemap.m_W, cubemap.m_H);

                glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(m_Handle, GL_TEXTURE_BASE_LEVEL, 0);
                glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
                glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
                glTextureStorage2D(m_Handle, numMipmaps, GL_RGB32F, cubemap.m_W, cubemap.m_H);
                const uint8_t* data = cubemap.m_Data.data();

                for (unsigned i = 0; i != 6; ++i)
                {
                    glTextureSubImage3D(m_Handle, 0, 0, 0, i, cubemap.m_W, cubemap.m_H, 1, GL_RGB, GL_FLOAT, data);
                    data += cubemap.m_W * cubemap.m_H * cubemap.m_Comp * Bitmap::GetBytesPerComponent(cubemap.m_Fmt);
                }
                glGenerateTextureMipmap(m_Handle);
                break;
            }
        default:
            CIAO_ASSERT(false, "No comparable texture format.");
        }

        // m_HandleBindless = glGetTextureHandleARB(m_Handle);
        // glMakeTextureHandleResidentARB(m_HandleBindless);
    }

    Texture::Texture(int w, int h, const void* img)
        : m_Type(GL_TEXTURE_2D)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glCreateTextures(m_Type, 1, &m_Handle);
        int numMipmaps = getNumMipMapLevels2D(w, h);
        glTextureStorage2D(m_Handle, numMipmaps, GL_RGBA8, w, h);
        glTextureSubImage2D(m_Handle, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
        glGenerateTextureMipmap(m_Handle);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
        glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_Handle, GL_TEXTURE_MAX_ANISOTROPY, 16);
        // m_HandleBindless = glGetTextureHandleARB(m_Handle);
        // glMakeTextureHandleResidentARB(m_HandleBindless);
    }

    Texture::~Texture()
    {
        // if (m_HandleBindless)
        //     glMakeTextureHandleNonResidentARB(m_HandleBindless);
        glDeleteTextures(1, &m_Handle);
    }

    Texture::Texture(Texture&& other)
        : m_Type(other.m_Type), m_Handle(other.m_Handle)
    {
        other.m_Type = 0;
        other.m_Handle = 0;
        //other.m_HandleBindless = 0;
    }
}
