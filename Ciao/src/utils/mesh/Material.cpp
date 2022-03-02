#include "pch.h"
#include "Material.h"

#include "utils/ReadFile.h"

namespace Ciao
{
    void SaveMaterials(const char* fileName, const std::vector<MaterialDescription>& materials, const std::vector<std::string>& files)
    {
        Timer timer;
        FILE* f = fopen(fileName, "wb");
        if (!f)
        {
            CIAO_CORE_ERROR("Cannot save material file {}.", fileName);
            exit(EXIT_FAILURE);
        }

        uint32_t sz = (uint32_t)materials.size();
        fwrite(&sz, 1, sizeof(uint32_t), f);
        fwrite(materials.data(), sizeof(MaterialDescription), sz, f);
        SaveStringList(f, files);
        fclose(f);

        CIAO_CORE_TRACE("Saving materials successfully! It costs {}.", timer.elapsedString());
    }
    
    void LoadMaterials(const char* fileName, std::vector<MaterialDescription>& materials, std::vector<std::string>& files)
    {
        Timer timer;
        FILE* f = fopen(fileName, "rb");
        if (!f)
        {
            CIAO_CORE_ERROR("Cannot load material file {}.", fileName);
            exit(EXIT_FAILURE);
        }

        uint32_t sz;
        fread(&sz, 1, sizeof(uint32_t), f);
        materials.resize(sz);
        fread(materials.data(), sizeof(MaterialDescription), materials.size(), f);
        LoadStringList(f, files);
        fclose(f);
        
        CIAO_CORE_TRACE("Saving materials successfully! It costs {}.", timer.elapsedString());
    }
    
    void MergeMaterialLists(
        // Input:
        const std::vector<std::vector<MaterialDescription>*>& oldMaterials,
        const std::vector<std::vector<std::string>*>& oldTextures,
        // Output:
        std::vector<MaterialDescription>& allMaterials,
        std::vector<std::string>& newTextures
    )
    {
        Timer timer;
        // ----- 首先合并材质 -----
        
        // 因为不能直接使用 MaterialDescription 作为键，所以使用它在 AllMaterials 数组中的索引
        std::unordered_map<int, int> materialToTextureList;

        // 创建合并材质的列表（没有进行哈希散列，只是直接合并所有列表）
        int matIdx = 0;
        for (const std::vector<MaterialDescription>* ml : oldMaterials) {
            for (const MaterialDescription& m : *ml) {
                allMaterials.push_back(m);
                materialToTextureList[allMaterials.size() - 1] = matIdx;
            }

            ++matIdx;
        }
        
        // ----- 再合并所有纹理 -----
        
        // 新纹理的名称和索引映射
        std::unordered_map<std::string, int> newTextureNames;
        // 遍历所有的材质，合并到材质列表中，并记录它们在列表中的索引
        for (const std::vector<std::string>* tl : oldTextures)
            for (const std::string& file : *tl) {
                newTextureNames[file] = AddUnique(newTextures, file);
            }
        

        // 对于每个材质，更新它的纹理索引
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
            replaceTexture(i, &m.baseColorTexId);
            replaceTexture(i, &m.metallicRoughnessTexID);
            replaceTexture(i, &m.normalmapTexID);
            replaceTexture(i, &m.emissionmapTexID);
        }

        CIAO_CORE_TRACE("Merging material successfully! It costs {}.", timer.elapsedString());
    }
}
