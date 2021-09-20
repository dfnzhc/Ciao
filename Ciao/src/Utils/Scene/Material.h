#pragma once

#include <cstdint>
#include "vec4.h"

#include <string>
#include <vector>

namespace Ciao
{
    enum MaterialFlags
    {
        sMaterialFlags_CastShadow = 0x1,
        sMaterialFlags_ReceiveShadow = 0x2,
        sMaterialFlags_Transparent = 0x4,
    };

    constexpr const uint64_t INVALID_TEXTURE = 0xFFFFFFFF;

    struct PACKED_STRUCT MaterialDescription final
    {
        gpuvec4 emissiveColor_ = {0.0f, 0.0f, 0.0f, 0.0f};
        gpuvec4 albedoColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
        // UV anisotropic roughness (isotropic lighting models use only the first value). ZW values are ignored
        // UV 各异向性的粗糙度（各向同性只使用第一个值），ZW 的值被忽略
        gpuvec4 roughness_ = {1.0f, 1.0f, 0.0f, 0.0f};
        float transparencyFactor_ = 1.0f;
        float alphaTest_ = 0.0f;
        float metallicFactor_ = 0.0f;
        uint32_t flags_ = sMaterialFlags_CastShadow | sMaterialFlags_ReceiveShadow;

        // maps
        uint64_t ambientOcclusionMap_ = INVALID_TEXTURE;
        uint64_t emissiveMap_ = INVALID_TEXTURE;
        uint64_t albedoMap_ = INVALID_TEXTURE;
        /// Occlusion (R), Roughness (G), Metallic (B) https://github.com/KhronosGroup/glTF/issues/857
        uint64_t metallicRoughnessMap_ = INVALID_TEXTURE;

        uint64_t normalMap_ = INVALID_TEXTURE;
        uint64_t opacityMap_ = INVALID_TEXTURE;
    };

    static_assert(sizeof(MaterialDescription) % 16 == 0, "MaterialDescription should be padded to 16 bytes");

    void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files);
    void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files);

    // Merge material lists from multiple scenes (follows the logic of merging in mergeScenes)
    // 从多个场景中整合出材质表
    void mergeMaterialLists(
        // Input:
        const std::vector<std::vector<MaterialDescription>*>& oldMaterials, // all materials
        const std::vector<std::vector<std::string>*>& oldTextures, // all textures from all material lists
        // Output:
        std::vector<MaterialDescription>& allMaterials,
        std::vector<std::string>& newTextures // all textures (merged from oldTextures, only unique items)
    );
}
