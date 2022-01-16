#pragma once

namespace Ciao
{
    enum AlphaMode
    {
        Opaque = 0, Blend, Mask
    };

    struct Material
    {
        vec3 baseColor{1, 1, 1};
        float anisotropic = 0;

        vec3 emission{0, 0, 0};
        float padding_1 = 0;

        float metallic = 0.0f;
        float roughness = 0.5f;
        float subsurface = 0.0f;
        float specularTint = 0.0f;

        float sheen = 0.0f;
        float sheenTint = 0.0f;
        float clearcoat = 0.0f;
        float clearcoatGloss = 0.0f;

        float specTrans = 0.0f;
        float ior = 1.5f;
        float attenDist = 1.0f;
        float padding_2 = 0;

        vec3 extinction{1, 1, 1};
        float padding_3 = 0;

        float baseColorTexId = -1.0f;
        float metallicRoughnessTexId = -1.0f;
        float normalMapTexId = -1.0f;
        float emissionMapTexId = -1.0f;

        float opacity = 1.0f;
        float alphaMode = AlphaMode::Opaque;
        float alphaCutoff = 0.0f;
        float padding_4 = 0;
    };
}
