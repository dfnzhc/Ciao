#pragma once

namespace Ciao
{
    constexpr const uint64_t INVALID_TEXTURE = 0xFFFFFFFF;
    
    struct MaterialDesc final
    {
        vec3 baseColor{1.0f, 1.0f, 1.0f};
        float anisotropic = 0.0f;

        vec3 emission{.0f, .0f, .0f};
        float metallic      = 0.0f;
        
        float roughness     = 0.5f;
        float opacity       = 1.0f;
        float alphaMode     = 0.0f;
        float alphaCutoff   = 0.0f;
        
        uint64_t albedoTexId = INVALID_TEXTURE;
        uint64_t metallicRoughnessTexID = INVALID_TEXTURE;
        uint64_t normalTexID = INVALID_TEXTURE;
        uint64_t emissionTexID = INVALID_TEXTURE;
        uint64_t opacityTexID = INVALID_TEXTURE;
        uint64_t ambientOcclusionTexID = INVALID_TEXTURE;
    };

    static_assert(sizeof(MaterialDesc) % 16 == 0, "MaterialDescription should be padded to 16 bytes");

    void SaveMaterials(const char* fileName, const std::vector<MaterialDesc>& materials, const std::vector<std::string>& files);
    bool LoadMaterials(const char* fileName, std::vector<MaterialDesc>& materials, std::vector<std::string>& files);

    // 合并多个材质及纹理，材质使用 MaterialDescription 记录，而纹理使用文件名
    void MergeMaterialLists(
        // Input:
        const std::vector<std::vector<MaterialDesc>*>& oldMaterials, // all materials
        const std::vector<std::vector<std::string>*>& oldTextures,          // all textures from all material lists
        // Output:
        std::vector<MaterialDesc>& allMaterials,
        std::vector<std::string>& newTextures           // all textures (merged from oldTextures, only unique items)
    );
}
