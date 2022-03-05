#include "pch.h"
#include "Scene.h"

#include "utils/mesh/MeshLoader.h"

namespace Ciao
{
    Scene::Scene()
    {
        
    }

    Scene::~Scene()
    {
        
    }

    void Scene::LoadMeshes()
    {
        for (const auto& cig : meshConfigs)
        {
            CIAO_INFO("Loading mesh {}...", cig.name);
            
            MeshData mesh;
            MeshFileHeader header;
            if (LoadMeshData(Res(cig.meshDataPath.c_str()), mesh, header))
            {
                CIAO_INFO("Loading mesh file {} Successfully!", cig.meshDataPath);
            }
            else
            {
                // LoadMeshFromFile(mesh, Res(cig.source.c_str()));
                // SaveMeshData(mesh, Res(cig.meshDataPath.c_str()));
            }

            //meshes.emplace_back(GLMesh{header, mesh.meshes.data(), mesh.indexData.data(), mesh.vertexData.data()});
        }
    }
}
