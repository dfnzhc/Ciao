#include "pch.h"
#include "CubemapUtils.h"


namespace Ciao
{
    vec3 texCoordsToXYZ(int i, int j, int faceID, int faceSize)
    {
        const float Pi = 2.0f * float(i) / faceSize;
        const float Pj = 2.0f * float(j) / faceSize;

        if (faceID == 0) return vec3(Pi - 1.0f, -1.0f, 1.0f - Pj);
        if (faceID == 1) return vec3(1.0f, Pi - 1.0f, 1.0f - Pj);
        if (faceID == 2) return vec3(1.0f - Pi, 1.0f, 1.0f - Pj);
        if (faceID == 3) return vec3(-1.0f, 1.0 - Pi, 1.0 - Pj);
        if (faceID == 4) return vec3(Pj - 1.0f, Pi - 1.0f, 1.0f);
        if (faceID == 5) return vec3(1.0f - Pj, Pi - 1.0f, -1.0f);

        return vec3();
    }

    Bitmap ConvertEquirectangularMapToVerticalCross(const Bitmap& b)
    {
        if (b.type_ != BitmapType::_2D) return Bitmap();

        const int faceSize = b.w_ / 4;

        const int w = faceSize * 4;
        const int h = faceSize * 3;

        Bitmap result(w, h, b.comp_, b.fmt_);

        const ivec2 kFaceOffsets[] =
        {
            ivec2(0, faceSize),
            ivec2(faceSize, faceSize),
            ivec2(2 * faceSize, faceSize),
            ivec2(3 * faceSize, faceSize),
            ivec2(faceSize, 0),
            ivec2(faceSize, 2 * faceSize)
        };

        const int clampW = b.w_ - 1;
        const int clampH = b.h_ - 1;


        for (int face = 0; face != 6; face++)
        {
            tbb::parallel_for(0, faceSize,
                              [&result, &kFaceOffsets, &b, face, faceSize, clampW, clampH](int i)
                              {
                                  for (int j = 0; j != faceSize; j++)
                                  {
                                      const vec3 P = texCoordsToXYZ(i, j, face, faceSize);
                                      const float R = hypot(P.x, P.y);
                                      const float theta = atan2(P.y, P.x);
                                      const float phi = atan2(P.z, R);
                                      //	float point source coordinates
                                      const float Uf = float(2.0f * faceSize * (theta + Pi) / Pi);
                                      const float Vf = float(2.0f * faceSize * (Pi / 2.0f - phi) / Pi);
                                      // 4-samples for bilinear interpolation
                                      const int U1 = glm::clamp(int(floor(Uf)), 0, clampW);
                                      const int V1 = glm::clamp(int(floor(Vf)), 0, clampH);
                                      const int U2 = glm::clamp(U1 + 1, 0, clampW);
                                      const int V2 = glm::clamp(V1 + 1, 0, clampH);
                                      // fractional part
                                      const float s = Uf - U1;
                                      const float t = Vf - V1;
                                      // fetch 4-samples
                                      const vec4 A = b.getPixel(U1, V1);
                                      const vec4 B = b.getPixel(U2, V1);
                                      const vec4 C = b.getPixel(U1, V2);
                                      const vec4 D = b.getPixel(U2, V2);
                                      // bilinear interpolation
                                      const vec4 color = A * (1 - s) * (1 - t) + B * (s) * (1 - t) + C * (1 - s) * t + D
                                          * (s) * (t);
                                      result.setPixel(i + kFaceOffsets[face].x, j + kFaceOffsets[face].y, color);
                                  }
                              });
        }

        return result;
    }

    Bitmap ConvertVerticalCrossToCubeMapFaces(const Bitmap& b)
    {
        const int faceWidth = b.w_ / 4;
        const int faceHeight = b.h_ / 3;

        Bitmap cubemap(faceWidth, faceHeight, 6, b.comp_, b.fmt_);
        cubemap.type_ = BitmapType::_Cube;

        const uint8_t* src = b.data_.data();
        uint8_t* dst = cubemap.data_.data();

        /*
              ------
              | +Y |
         ---------------------
         | -X | -Z | +X | +Z |
         ---------------------
              | -Y |
              ------
        */
        const int pixelSize = cubemap.comp_ * Bitmap::getBytesPerComponent(cubemap.fmt_);
        for (int face = 0; face != 6; ++face)
        {
            for (int j = 0; j != faceHeight; ++j)
            {
                for (int i = 0; i != faceWidth; ++i)
                {
                    int x = 0;
                    int y = 0;
        
                    switch (face)
                    {
                    // GL_TEXTURE_CUBE_MAP_POSITIVE_X
                    case 0:
                        x = i;
                        y = faceHeight + j;
                        break;
        
                    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
                    case 1:
                        x = 2 * faceWidth + i;
                        y = 1 * faceHeight + j;
                        break;
        
                    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
                    case 2:
                        x = 2 * faceWidth - (i + 1);
                        y = 1 * faceHeight - (j + 1);
                        break;
        
                    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
                    case 3:
                        x = 2 * faceWidth - (i + 1);
                        y = 3 * faceHeight - (j + 1);
                        break;
        
                    // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
                    case 4:
                        x = 3 * faceWidth + i;
                        y = 1 * faceHeight + j;
                        break;
        
                    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
                    case 5:
                        x = faceWidth + i;
                        y = faceHeight + j;
                        break;
                    }
        
                    memcpy(dst, src + (y * b.w_ + x) * pixelSize, pixelSize);
        
                    dst += pixelSize;
                }
            }
        }

        return cubemap;
    }
}
