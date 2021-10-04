#pragma once

namespace Ciao
{
    struct MeshData;
    struct Scene;

    void mergeScene(Scene& scene, MeshData& meshData, const std::string& materialName);
}