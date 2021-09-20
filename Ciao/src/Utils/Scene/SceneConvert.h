#pragma once

namespace Ciao
{
    const uint32_t g_numElementsToStore = 3 + 3 + 2; // pos(vec3) + normal(vec3) + uv(vec2)

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

    void SceneConvert(const char* sceneConfigFile);
}
