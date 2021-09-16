#pragma once

#include "glm/glm.hpp"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "gli/gli.hpp"

#include "UtilsMath.h"

namespace Ciao
{
    //--------------------------------------------------------------------------------------
    // 工具函数
    //--------------------------------------------------------------------------------------
    /// 按比特反转
    /// From Henry J. Warren's "Hacker's Delight"
    float RadicalInverse_VdC(uint32_t bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
    }

    // Hammersley 是一种低差异度序列，用来使采样更加均匀
    glm::vec2 Hammersley2d(uint32_t i, uint32_t N)
    {
        return glm::vec2(float(i)/float(N), RadicalInverse_VdC(i));
    }

    //--------------------------------------------------------------------------------------
    // 对环境贴图的预处理
    //--------------------------------------------------------------------------------------
    void PrefilterEnvMap(const glm::vec3* data, int srcW, int srcH, int dstW, int dstH, glm::vec3* output, int numMonteCarloSamples)
    {
        CIAO_ASSERT(srcW == 2 * srcH, "only equirectangular maps are supported");

        if (srcW != 2 * srcH) return;

        std::vector<glm::vec3> tmp(dstW * dstH);

        stbir_resize_float_generic(
            reinterpret_cast<const float*>(data), srcW, srcH, 0,
            reinterpret_cast<float*>(tmp.data()), dstW, dstH, 0, 3,
            STBIR_ALPHA_CHANNEL_NONE, 0, STBIR_EDGE_CLAMP, STBIR_FILTER_CUBICBSPLINE, STBIR_COLORSPACE_LINEAR, nullptr);

        const glm::vec3* scratch = tmp.data();
        srcW = dstW;
        srcH = dstH;

        for (int y = 0; y != dstH; y++)
        {
            const float theta1 = float(y) / float(dstH) * Math::PI;
            for (int x = 0; x != dstW; x++)
            {
                const float phi1 = float(x) / float(dstW) * Math::TWOPI;
                const glm::vec3 V1 = glm::vec3(sin(theta1) * cos(phi1), sin(theta1) * sin(phi1), cos(theta1));
                glm::vec3 color = glm::vec3(0.0f);
                float weight = 0.0f;
                for (int i = 0; i != numMonteCarloSamples; i++)
                {
                    const glm::vec2 h = Hammersley2d(i, numMonteCarloSamples);
                    const int x1 = int(floor(h.x * srcW));
                    const int y1 = int(floor(h.y * srcH));
                    const float theta2 = float(y1) / float(srcH) * Math::PI;
                    const float phi2 = float(x1) / float(srcW) * Math::TWOPI;
                    const glm::vec3 V2 = glm::vec3(sin(theta2) * cos(phi2), sin(theta2) * sin(phi2), cos(theta2));
                    const float D = std::max(0.0f, glm::dot(V1, V2));
                    if (D > 0.01f)
                    {
                        color += scratch[y1 * srcW + x1] * D;
                        weight += D;
                    }
                }
                output[y * dstW + x] = color / weight;
            }
        }
    }

    void Process_EnvMap(const char* filename, const char* dstFilename, int numSample = 1024)
    {
        int w, h, comp;
        const float* img = stbi_loadf(filename, &w, &h, &comp, 3);

        if (!img)
        {
            CIAO_CORE_ERROR("Failed to load {} texture.", filename);
            return;
        }

        const int dstW = 256;
        const int dstH = 128;

        std::vector<glm::vec3> out(dstW * dstH);

        PrefilterEnvMap((glm::vec3*)img, w, h, dstW, dstH, out.data(), numSample);

        stbi_image_free((void*)img);
        stbi_write_hdr(dstFilename, dstW, dstH, 3, (float*)out.data());
    }

    //--------------------------------------------------------------------------------------
    // 预计算 BRDF 项
    //--------------------------------------------------------------------------------------
    
    // 重要性采样
    glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, float roughness, glm::vec3 N)
    {
        float a = roughness * roughness;

        float phi = 2.0 * Math::PI * Xi.x;
        float cosTheta = glm::sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
        float sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

        // 转换到笛卡尔坐标系
        glm::vec3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // 将在切线空间的采样向量，转换到世界坐标
        glm::vec3 up = abs(N.z) < 0.999 ? glm::vec3 (0.0, 0.0, 1.0) : glm::vec3(1.0, 0.0, 0.0);
        glm::vec3 tangent = glm::normalize(glm::cross(up, N));
        glm::vec3 bitangent = cross(N, tangent);

        glm::vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return glm::normalize(sampleVec);
    }

    //--------------------------------------------------------------------------------------
    // schlick 模型的几何项
    // 原文中 k = a * a / 2, Epic 使用的 k = a / 2，理由是更加适用于 GGX smith 模型
    //--------------------------------------------------------------------------------------
    float GeometrySchlickGGX(float NdotV, float roughness)
    {
        float a = roughness;
        float k = (a * a) * 0.5f;

        float nom = NdotV;
        float denom = NdotV * (1.0 - k) + k;

        return nom / denom;
    }

    float GeometrySmith(float roughness, float NoV, float NoL)
    {
        float ggx2 = GeometrySchlickGGX(NoV, roughness);
        float ggx1 = GeometrySchlickGGX(NoL, roughness);

        return ggx1 * ggx2;
    }

    glm::vec2 IntegrateBRDF(float NdotV, float roughness, unsigned int samples = 1024)
    {
        glm::vec3 V;
        V.x = sqrt(1.0 - NdotV * NdotV);
        V.y = 0.0;
        V.z = NdotV;

        float A = 0.0;
        float B = 0.0;

        glm::vec3 N = glm::vec3(0.0, 0.0, 1.0);

        for (unsigned int i = 0u; i < samples; i++)
        {
            glm::vec2 Xi = Hammersley2d(i, samples);
            glm::vec3 H = ImportanceSampleGGX(Xi, roughness, N);
            glm::vec3 L = normalize(2.0f * dot(V, H) * H - V);

            float NoL = glm::max(L.z, 0.0f);
            float NoH = glm::max(H.z, 0.0f);
            float VoH = glm::max(dot(V, H), 0.0f);
            float NoV = glm::max(dot(N, V), 0.0f);

            if (NoL > 0.0)
            {
                float G = GeometrySmith(roughness, NoV, NoL);

                float G_Vis = (G * VoH) / (NoH * NoV);
                float Fc = pow(1.0 - VoH, 5.0);

                A += (1.0 - Fc) * G_Vis;
                B += Fc * G_Vis;
            }
        }
        return glm::vec2(A / float(samples), B / float(samples));
    }

    void Compute_brdfLUT(const char* filename, int texSize = 256, unsigned int numBits = 16, unsigned int numSample = 1024)
    {
        gli::texture2d tex;

        if(numBits == 16)
            tex = gli::texture2d(gli::FORMAT_RG16_SFLOAT_PACK16, gli::extent2d(texSize, texSize), 1);
        if(numBits == 32)
            tex = gli::texture2d(gli::FORMAT_RG32_SFLOAT_PACK32, gli::extent2d(texSize, texSize), 1);

        for (int y = 0; y < texSize; y++)
        {
            for (int x = 0; x < texSize; x++)
            {
                float NoV = (y + 0.5f) * (1.0f / texSize);
                float roughness = (x + 0.5f) * (1.0f / texSize);

                if (numBits == 16)
                    tex.store<glm::uint32>({ y, texSize - 1 - x }, 0, gli::packHalf2x16(IntegrateBRDF(NoV, roughness, numSample)));
                if (numBits == 32)
                    tex.store<glm::vec2>({ y, texSize - 1 - x }, 0, IntegrateBRDF(NoV, roughness,numSample));
            }
        }

        gli::save(tex, filename);

        CIAO_CORE_INFO("{} bit, {}x{} BRDF LUT generated using {} samples.", numBits, texSize, texSize, numSample);
        CIAO_CORE_INFO("Saved LUT to {}", filename);
    }
}
