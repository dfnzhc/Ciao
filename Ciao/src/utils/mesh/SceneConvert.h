#pragma once
#include "Material.h"

namespace Ciao
{
    struct MeshData;
    struct SceneGraph;
    
    void MergeScene(SceneGraph& scene, MeshData& meshData, const std::string& materialName);

    struct SceneConfig
    {
        std::string fileName;
        std::string outputMesh;
        std::string outputScene;
        std::string outputMaterials;
        float scale;
        bool calculateLODs;
        bool mergeInstances;
    };

    void SceneConvert(const std::string& sceneConfigFile);

    void ProcessScene(const SceneConfig& cfg);

    MaterialDescription ConvertAIMaterialToDescription(const aiMaterial* M, std::vector<std::string>& files, std::vector<std::string>& opacityMaps);

    void ConvertAndDownscaleAllTextures(
        const std::vector<MaterialDescription>& materials, const std::string& basePath, std::vector<std::string>& files, std::vector<std::string>& opacityMaps
    );

    std::string ConvertTexture(const std::string& file, const std::string& basePath, std::unordered_map<std::string, uint32_t>& opacityMapIndices,
        const std::vector<std::string>& opacityMaps);

    void Traverse(const aiScene* sourceScene, SceneGraph& scene, aiNode* N, int parent, int ofs);
}
