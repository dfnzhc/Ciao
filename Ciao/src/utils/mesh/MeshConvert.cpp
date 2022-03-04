#include "pch.h"
#include "MeshConvert.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>

namespace Ciao
{
    void LoadMeshFromFile(MeshData& meshData, const char* fileName)
    {
        Timer timer;

        const unsigned int flags = 0 |
            aiProcess_JoinIdenticalVertices |
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_LimitBoneWeights |
            aiProcess_SplitLargeMeshes |
            aiProcess_ImproveCacheLocality |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_FindDegenerates |
            aiProcess_FindInvalidData |
            aiProcess_GenUVCoords;

        const aiScene* scene = aiImportFile(fileName, flags);

        if (!scene || !scene->HasMeshes())
        {
            CIAO_CORE_ERROR("Unable to load '{}'", fileName);
            exit(EXIT_FAILURE);
        }

        meshData.meshes.reserve(scene->mNumMeshes);

        uint32_t indexOffset = 0;
        uint32_t vertexOffset = 0;
        for (unsigned int i = 0; i != scene->mNumMeshes; i++)
        {
            CIAO_INFO("Loading meshes {}/{}...", i + 1, scene->mNumMeshes);
            meshData.meshes.push_back(LoadAIMesh(scene->mMeshes[i], meshData.vertexData, meshData.indexData,
                indexOffset, vertexOffset));
        }

        CIAO_INFO("Loading {} successfully! It costs {}.", fileName, timer.elapsedString());
    }

    Mesh LoadAIMesh(const aiMesh* m, std::vector<float>& vertices, std::vector<uint32_t>& indices,
        uint32_t& idxOffset, uint32_t& vertOffset)
    {
        const bool hasTexCoords = m->HasTextureCoords(0);

        for (size_t i = 0; i != m->mNumVertices; i++)
        {
            const aiVector3D v = m->mVertices[i];
            const aiVector3D n = m->mNormals[i];
            const aiVector3D t = hasTexCoords ? m->mTextureCoords[0][i] : aiVector3D();

            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);

            vertices.push_back(t.x);
            vertices.push_back(1.0f - t.y);

            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }

        // 加载索引
        uint32_t numIndices = 0;
        for (size_t i = 0; i != m->mNumFaces; i++)
        {
            if (m->mFaces[i].mNumIndices != 3)
                continue;
            
            for (unsigned j = 0; j != m->mFaces[i].mNumIndices; j++)
            {
                indices.push_back(m->mFaces[i].mIndices[j]);
                numIndices += 1;
            }
        }

        Mesh result = {
            .indexOffset = idxOffset,
            .vertexOffset = vertOffset,
            .vertexCount = m->mNumVertices,
            .indexCount = numIndices
        };
        
        idxOffset += numIndices;
        vertOffset += m->mNumVertices;
        
        return result;
    }

    void SaveDrawData(const MeshData& meshData, const char* fileName)
    {
        Timer timer;
        std::vector<DrawData> grid;
        uint32_t vertexOffset = 0;
        for (auto i = 0; i < meshData.meshes.size(); i++)
        {
            grid.push_back(DrawData{
                .meshIndex = (uint32_t)i,
                .materialIndex = 0,
                .indexOffset = meshData.meshes[i].indexOffset,
                .vertexOffset = vertexOffset,
                });
            vertexOffset += meshData.meshes[i].vertexCount;
        }

        FILE* f = fopen(fileName, "wb");
        fwrite(grid.data(), grid.size(), sizeof(DrawData), f);

        fclose(f);

        CIAO_INFO("Saving draw data successfully! It cost {}.", timer.elapsedString());
    }
}