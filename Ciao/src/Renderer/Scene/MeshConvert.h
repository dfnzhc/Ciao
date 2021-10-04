#pragma once

#include <assimp/scene.h>

#include "VtxData.h"

namespace Ciao
{
    constexpr uint32_t g_numElementsToStore = 3 + 3 + 2;

    void ProcessLods(std::vector<uint32_t>& indices, std::vector<float>& vertices,
        std::vector<std::vector<uint32_t>>& outLods);

    Mesh ConvertAIMesh(const aiMesh* m, std::vector<float>& vertices, std::vector<uint32_t>& indices,
        uint32_t& idxOffset, uint32_t& vertOffset, float scale = 0.01f, bool calLODs = true);

    void LoadFile(MeshData& meshData, const char* fileName);

    void SaveDrawData(const MeshData& meshData, const char* fileName);
}
