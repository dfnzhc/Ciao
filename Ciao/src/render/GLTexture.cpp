#include "pch.h"
#include "GLTexture.h"
#include "stb_image.h"

namespace Ciao
{
    GLTexture::GLTexture(std::string texName, unsigned char* data, int w, int h, int c)
        : width_(w), height_(h), components_(c)
    {
        data_.resize(w * h * c);
        std::copy(data, data + w * h * c, data_.begin());
    }

    bool GLTexture::loadTexture(const std::string_view& filename)
    {
        name_ = filename;
        components_ = 4;

        unsigned char* data = stbi_load(name_.c_str(), &width_, &height_, NULL, components_);
        if (data == nullptr)
            return false;
        data_.resize(width_ * height_ * components_);
        std::copy(data, data + width_ * height_ * components_, data_.begin());
        stbi_image_free(data);
        return true;
    }
}
