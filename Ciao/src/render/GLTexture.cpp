#include "pch.h"
#include "GLTexture.h"
#include "stb_image.h"
#include "utils/Bitmap.h"
#include "utils/CubemapUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ciao
{
    int getNumMipMapLevels2D(int w, int h)
    {
        int levels = 1;
        while ((w | h) >> levels)
            levels += 1;
        return levels;
    }


    GLTexture::GLTexture(GLenum type, int width, int height, GLenum internalFormat)
        : type_(type)
    {
        glCreateTextures(type, 1, &handle_);
        glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureStorage2D(handle_, getNumMipMapLevels2D(width, height), internalFormat, width, height);
    }

    /// Draw a checkerboard on a pre-allocated square RGB image.
    uint8_t* genDefaultCheckerboardImage(int* width, int* height)
    {
        const int w = 128;
        const int h = 128;

        uint8_t* imgData = (uint8_t*)malloc(w * h * 3); // stbi_load() uses malloc(), so this is safe

        assert(imgData && w > 0 && h > 0);
        assert(w == h);

        if (!imgData || w <= 0 || h <= 0) return nullptr;
        if (w != h) return nullptr;

        for (int i = 0; i < w * h; i++)
        {
            const int row = i / w;
            const int col = i % w;
            imgData[i * 3 + 0] = imgData[i * 3 + 1] = imgData[i * 3 + 2] = 0xFF * ((row + col) % 2);
        }

        if (width) *width = w;
        if (height) *height = h;

        return imgData;
    }

    GLTexture::GLTexture(GLenum type, const char* fileName)
        : type_(type)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glCreateTextures(type, 1, &handle_);
        glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, 0);
        glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const char* ext = strrchr(fileName, '.');

        switch (type)
        {
        case GL_TEXTURE_2D:
            {
                int w = 0;
                int h = 0;
                int numMipmaps = 0;

                uint8_t* img = stbi_load(fileName, &w, &h, nullptr, STBI_rgb_alpha);

                // Note(Anton): replaced assert(img) with a fallback image to prevent crashes with missing files or bad (eg very long) paths.
                if (!img)
                {
                    CIAO_CORE_WARN("WARNING: could not load image `{}`, using a fallback.", fileName);
                    img = genDefaultCheckerboardImage(&w, &h);
                    if (!img)
                    {
                        CIAO_CORE_ERROR("FATAL ERROR: out of memory allocating image for fallback texture.");
                        exit(EXIT_FAILURE);
                    }
                }

                numMipmaps = getNumMipMapLevels2D(w, h);
                glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
                glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
                stbi_image_free((void*)img);

                glGenerateTextureMipmap(handle_);
                glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
                glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY, 16);
                break;
            }
        case GL_TEXTURE_CUBE_MAP:
            {
                std::string fName = fileName;
                fName.erase(std::find(fName.begin(), fName.end(), '.'), fName.end());
                
                int w, h, comp;
                const float* img = stbi_loadf((fName + "_Cross.hdr").c_str(), &w, &h, &comp, 3);
                Bitmap crossMap;
                if (img == nullptr)
                {
                    img = stbi_loadf(fileName, &w, &h, &comp, 3);
                    CIAO_ASSERT(img, "Load Cubemap Faild!");
                    Bitmap in(w, h, comp, BitmapFormat::Float, img);
                    const bool isEquirectangular = w == 2 * h;
                    
                    crossMap = isEquirectangular ? ConvertEquirectangularMapToVerticalCross(in) : in;
                    crossMap.SaveEXR(fName + "_Cross.hdr");
                } else
                {
                    crossMap = Bitmap(w, h, comp, BitmapFormat::Float, img);
                } 
                
                stbi_image_free((void*)img);
                
                Bitmap cubemap = ConvertVerticalCrossToCubeMapFaces(crossMap);

                const int numMipmaps = getNumMipMapLevels2D(cubemap.w_, cubemap.h_);

                glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTextureParameteri(handle_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(handle_, GL_TEXTURE_BASE_LEVEL, 0);
                glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
                glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
                glTextureStorage2D(handle_, numMipmaps, GL_RGB32F, cubemap.w_, cubemap.h_);
                const uint8_t* data = cubemap.data_.data();

                for (unsigned i = 0; i != 6; ++i)
                {
                    glTextureSubImage3D(handle_, 0, 0, 0, i, cubemap.w_, cubemap.h_, 1, GL_RGB, GL_FLOAT, data);
                    data += cubemap.w_ * cubemap.h_ * cubemap.comp_ * Bitmap::getBytesPerComponent(cubemap.fmt_);
                }

                glGenerateTextureMipmap(handle_);
                break;
            }
        default:
            assert(false);
        }

        handleBindless_ = glGetTextureHandleARB(handle_);
        glMakeTextureHandleResidentARB(handleBindless_);
    }

    GLTexture::GLTexture(int w, int h, const void* img)
        : type_(GL_TEXTURE_2D)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glCreateTextures(type_, 1, &handle_);
        int numMipmaps = getNumMipMapLevels2D(w, h);
        glTextureStorage2D(handle_, numMipmaps, GL_RGBA8, w, h);
        glTextureSubImage2D(handle_, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img);
        glGenerateTextureMipmap(handle_);
        glTextureParameteri(handle_, GL_TEXTURE_MAX_LEVEL, numMipmaps - 1);
        glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(handle_, GL_TEXTURE_MAX_ANISOTROPY, 16);
        handleBindless_ = glGetTextureHandleARB(handle_);
        glMakeTextureHandleResidentARB(handleBindless_);
    }

    GLTexture::GLTexture(GLTexture&& other)
        : type_(other.type_)
          , handle_(other.handle_)
          , handleBindless_(other.handleBindless_)
    {
        other.type_ = 0;
        other.handle_ = 0;
        other.handleBindless_ = 0;
    }

    GLTexture::~GLTexture()
    {
        if (handleBindless_)
            glMakeTextureHandleNonResidentARB(handleBindless_);
        glDeleteTextures(1, &handle_);
    }
}
