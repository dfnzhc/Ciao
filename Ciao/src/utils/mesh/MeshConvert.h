#pragma once
#include "MeshData.h"

namespace Ciao
{
    constexpr uint32_t NumElementsToStore = 3 + 3 + 2;

    void LoadMeshFromFile(MeshData& meshData, const char* fileName);

    Mesh LoadAIMesh(const aiMesh* m, std::vector<float>& vertices, std::vector<uint32_t>& indices,
        uint32_t& idxOffset, uint32_t& vertOffset);
    
    void SaveDrawData(const MeshData& meshData, const char* fileName);
}
