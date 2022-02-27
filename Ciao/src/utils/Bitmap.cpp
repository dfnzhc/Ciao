#include "pch.h"
#include "Bitmap.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace Ciao
{
    void Bitmap::SaveEXR(const std::string& filename)
    {
        CIAO_CORE_INFO("Writing a {} x {} OpenEXR file to {}...", w_, h_, filename);

        int ret = stbi_write_hdr(filename.c_str(), w_, h_, comp_, (const float*)data_.data());
        if (ret == 0) {
            CIAO_CORE_ERROR("Bitmap::SaveEXR(): Could not save EXR file {}.", filename);
        }
    }

    void Bitmap::SavePNG(const std::string& filename)
    {
        CIAO_CORE_INFO("Writing a {} x {} PNG file to {}...", w_, h_, filename);

        uint8_t *rgb8 = new uint8_t[3 * w_ * h_];
        uint8_t *dst = rgb8;
        for (int i = 0; i < h_; ++i) {
            for (int j = 0; j < w_; ++j) {
                vec4 p = getPixel(j, i);
                dst[0] = (uint8_t) glm::clamp(255.f * p[0], 0.f, 255.f);
                dst[1] = (uint8_t) glm::clamp(255.f * p[1], 0.f, 255.f);
                dst[2] = (uint8_t) glm::clamp(255.f * p[2], 0.f, 255.f);
                dst += 3;
            }
        }
        
        int ret = stbi_write_png(filename.c_str(), w_, h_, 3, rgb8, 3 * (int)w_);
        if (ret == 0) {
            CIAO_CORE_ERROR("Bitmap::SavePNG(): Could not save PNG file {}.", filename);
        }
        
        delete[] rgb8;
    }
}
