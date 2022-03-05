#pragma once

namespace Ciao
{
    constexpr const uint64_t INVALID_TEXTURE = 0xFFFFFFFF;
    
    struct MaterialDescription final
    {
        vec3 baseColor{1.0f, 1.0f, 1.0f};
        float anisotropic = 0.0f;

        vec3 emission{.0f, .0f, .0f};
        float padding1 = 0.0f;

        float metallic      = 0.0f;
        float roughness     = 0.5f;
        float subsurface    = 0.0f;
        float specularTint  = 0.0f;

        float sheen          = 0.0f;
        float sheenTint      = 0.0f;
        float clearcoat      = 0.0f;
        float clearcoatGloss = 0.0f;

        float specTrans     = 0.0f;
        float ior           = 1.5f;
        float atDistance    = 1.0f;
        float padding2      = 0.0f;
        
        vec3 extinction{1.0f, 1.0f, 1.0f};
        float padding3 = 0.0f;

        float opacity       = 1.0f;
        float alphaMode     = 0.0f;
        float alphaCutoff   = 0.0f;
        float padding4      = 0.0f;
        
        uint64_t baseColorTexId = INVALID_TEXTURE;
        uint64_t metallicRoughnessTexID = INVALID_TEXTURE;
        uint64_t normalmapTexID = INVALID_TEXTURE;
        uint64_t emissionmapTexID = INVALID_TEXTURE;
        uint64_t opacityTexID = INVALID_TEXTURE;
        uint64_t ambientOcclusionTexID = INVALID_TEXTURE;
    };

    static_assert(sizeof(MaterialDescription) % 16 == 0, "MaterialDescription should be padded to 16 bytes");

    void SaveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files);
    bool LoadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files);

    // 合并多个材质及纹理，材质使用 MaterialDescription 记录，而纹理使用文件名
    void MergeMaterialLists(
        // Input:
        const std::vector<std::vector<MaterialDescription>*>& oldMaterials, // all materials
        const std::vector<std::vector<std::string>*>& oldTextures,          // all textures from all material lists
        // Output:
        std::vector<MaterialDescription>& allMaterials,
        std::vector<std::string>& newTextures           // all textures (merged from oldTextures, only unique items)
    );
}
