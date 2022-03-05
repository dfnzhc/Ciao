#pragma once
#include "Material.h"
#include "MeshData.h"

struct aiMaterial;

namespace Ciao
{
    struct MeshConfig;
    constexpr uint32_t NumElementsToStore = 3 + 3 + 2;

    void LoadMeshFromFile(MeshData& meshData, const MeshConfig& mesh_config);

    Mesh LoadAIMesh(const aiMesh* m, std::vector<float>& vertices, std::vector<uint32_t>& indices,
        uint32_t& idxOffset, uint32_t& vertOffset);
    
    MaterialDesc LoadAIMaterial(const aiMaterial* M, std::vector<std::string>& texFiles, std::vector<std::string>& OpacityTexFiles);

    void DownSamplingTextures(const std::vector<MaterialDesc>& materials, const std::string& path, const std::string& new_path,
        std::vector<std::string>& texFiles, std::vector<std::string>& opacityTexFiles);
    
    void SaveDrawData(const MeshData& meshData, const char* fileName);

}
