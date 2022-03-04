#pragma once
#include "GLMesh.h"
#include "RenderOption.h"
#include "utils/mesh/MeshData.h"

namespace Ciao
{
    
    
    class Scene
    {
    public:
        Scene();
        ~Scene();

        void LoadMeshes();
        
        RenderOptions renderOptions;

        CameraConfig cameraConfig;
        
        std::vector<MeshConfig> meshConfigs;
    private:
    };
}
