#pragma once

#include "stb_image.h"

namespace Ciao
{
    class EnvironmentMap
    {
    public:
        EnvironmentMap() : width(0), height(0), img(nullptr), cdf(nullptr) {};
        ~EnvironmentMap() { stbi_image_free(img); delete[] cdf; }

        bool LoadMap(const std::string& filename);
        void BuildCDF();

        int width;
        int height;
        float totalSum;
        float* img;
        float* cdf;
    };
}
