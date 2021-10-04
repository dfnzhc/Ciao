#include "pch.h"
#include "Material.h"

#include "Utils/Utils.h"

namespace Ciao
{
    void saveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files)
    {
        FILE* f = fopen(fileName, "wb");
        if (!f)
            return;

        uint32_t sz = (uint32_t)materials.size();
        fwrite(&sz, 1, sizeof(uint32_t), f);
        fwrite(materials.data(), sizeof(MaterialDescription), sz, f);
        saveStringList(f, files);
        fclose(f);
    }

    void loadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files)
    {
        FILE* f = fopen(fileName, "rb");
        if (!f)
        {
            CIAO_CORE_ERROR("Cannot load file {}.", fileName);
            exit(255);
        }

        uint32_t sz;
        fread(&sz, 1, sizeof(uint32_t), f);
        materials.resize(sz);
        fread(materials.data(), sizeof(MaterialDescription), materials.size(), f);
        loadStringList(f, files);
        fclose(f);
    }

    void mergeMaterialLists(
        const std::vector<std::vector<MaterialDescription>*>& oldMaterials,
        const std::vector<std::vector<std::string>*>& oldTextures,
        std::vector<MaterialDescription>& allMaterials,
        std::vector<std::string>& newTextures
    )
    {
        // map texture names to indices in newTexturesList (calculated as we fill the newTexturesList)
        // 将纹理名称映射到 newTextureList 中的索引
        std::unordered_map<std::string, int> newTextureNames;
        // direct MaterialDescription usage as a key is impossible, so we use its index in the allMaterials array
        // 直接使用 MaterialDescription 作为键是不可能的，所以使用它在 AllMaterials 数组中的索引
        std::unordered_map<int, int> materialToTextureList;

        // Create combined material list [no hashing of materials, just straightforward merging of all lists]
        // 创建合并材质的列表（没有进行哈希散列，只是直接合并所有列表）
        int midx = 0;
        for (const std::vector<MaterialDescription>* ml : oldMaterials) {
            for (const MaterialDescription& m : *ml) {
                allMaterials.push_back(m);
                materialToTextureList[allMaterials.size() - 1] = midx;
            }

            midx++;
        }

        // Create one combined texture list
        // 遍历所有的材质，合并到材质列表中，并记录它们在列表中的索引
        for (const std::vector<std::string>* tl : oldTextures)
            for (const std::string& file : *tl) {
                newTextureNames[file] = addUnique(newTextures, file);
            }

        // Lambda to replace textureID by a new "version" (from global list)
        // 用来将 textureID 替换成相对合并后列表的 ID 
        auto replaceTexture = [&materialToTextureList, &oldTextures, &newTextureNames](int m, uint64_t* textureID) {
            if (*textureID < INVALID_TEXTURE) {
                auto listIdx = materialToTextureList[m];
                auto texList = oldTextures[listIdx];
                const std::string& texFile = (*texList)[*textureID];
                *textureID = (uint64_t)(newTextureNames[texFile]);
            }
        };

        for (size_t i = 0; i < allMaterials.size(); i++)
        {
            auto& m = allMaterials[i];
            replaceTexture(i, &m.ambientOcclusionMap_);
            replaceTexture(i, &m.emissiveMap_);
            replaceTexture(i, &m.albedoMap_);
            replaceTexture(i, &m.metallicRoughnessMap_);
            replaceTexture(i, &m.normalMap_);
        }
    }
}
