#include "pch.h"
#include "MeshConvert.h"

namespace Ciao
{
    void LoadFile(MeshData& meshData, const char* fileName)
    {
        Timer timer;
        CIAO_CORE_INFO("Loading '{}' ... ", fileName);

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
            exit(255);
        }

        meshData.meshes.reserve(scene->mNumMeshes);
        meshData.boxes.reserve(scene->mNumMeshes);

        uint32_t indexOffset = 0;
        uint32_t vertexOffset = 0;
        for (unsigned int i = 0; i != scene->mNumMeshes; i++)
        {
            CIAO_CORE_INFO("Converting meshes {}/{}...", i + 1, scene->mNumMeshes);
            meshData.meshes.push_back(ConvertAIMesh(scene->mMeshes[i], meshData.vertexData, meshData.indexData,
                indexOffset, vertexOffset));
        }

        RecalculateBoundingBox(meshData);
        CIAO_CORE_TRACE("Loading {} successfully! It costs {}.", fileName, timer.elapsedString());
    }

    void ProcessLods(std::vector<uint32_t>& indices, std::vector<float>& vertices,
        std::vector<std::vector<uint32_t>>& outLods)
    {
        size_t verticesCountIn = vertices.size() / 3;
        size_t targetIndicesCount = indices.size();

        uint8_t LOD = 1;

        CIAO_CORE_INFO("    LOD0: {} indices", int(indices.size()));

        // 最低层次LOD（包括全部的索引）
        outLods.push_back(indices);

        while (targetIndicesCount > 1024 && LOD < 8)
        {
            targetIndicesCount = indices.size() / 2;

            bool sloppy = false;

            size_t numOptIndices = meshopt_simplify(
                indices.data(),
                indices.data(), (uint32_t)indices.size(),
                vertices.data(), verticesCountIn,
                sizeof(float) * 3,
                targetIndicesCount, 0.02f);

            // cannot simplify further
            if (static_cast<size_t>(numOptIndices * 1.1f) > indices.size())
            {
                if (LOD > 1)
                {
                    // try harder
                    numOptIndices = meshopt_simplifySloppy(
                        indices.data(),
                        indices.data(), indices.size(),
                        vertices.data(), verticesCountIn,
                        sizeof(float) * 3,
                        targetIndicesCount, 0.02f);
                    sloppy = true;
                    if (numOptIndices == indices.size()) break;
                }
                else
                    break;
            }

            indices.resize(numOptIndices);

            meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), verticesCountIn);

            CIAO_CORE_INFO("    LOD{}: {} indices {}.", int(LOD), int(numOptIndices), sloppy ? "[sloppy]" : "");

            LOD += 1;

            outLods.push_back(indices);
        }
    }

    Mesh ConvertAIMesh(const aiMesh* m, std::vector<float>& vertices, std::vector<uint32_t>& indices,
        uint32_t& idxOffset, uint32_t& vertOffset, float scale, bool calLODs)
    {
        const bool hasTexCoords = m->HasTextureCoords(0);
        const uint32_t streamElementSize = static_cast<uint32_t>(NumElementsToStore * sizeof(float));

        // Original data for LOD calculation
        std::vector<float> srcVertices;
        std::vector<uint32_t> srcIndices;

        std::vector<std::vector<uint32_t>> outLods;

        for (size_t i = 0; i != m->mNumVertices; i++)
        {
            const aiVector3D v = m->mVertices[i];
            const aiVector3D n = m->mNormals[i];
            const aiVector3D t = hasTexCoords ? m->mTextureCoords[0][i] : aiVector3D();

            if (calLODs)
            {
                srcVertices.push_back(v.x);
                srcVertices.push_back(v.y);
                srcVertices.push_back(v.z);
            }

            vertices.push_back(v.x * scale);
            vertices.push_back(v.y * scale);
            vertices.push_back(v.z * scale);

            vertices.push_back(t.x);
            vertices.push_back(1.0f - t.y);

            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }

        Mesh result = {
            .streamCount = 1,
            .indexOffset = idxOffset,
            .vertexOffset = vertOffset,
            .vertexCount = m->mNumVertices,
            .streamOffset = {vertOffset * streamElementSize},
            .streamElementSize = {streamElementSize}
        };

        // 加载索引
        for (size_t i = 0; i != m->mNumFaces; i++)
        {
            if (m->mFaces[i].mNumIndices != 3)
                continue;
            for (unsigned j = 0; j != m->mFaces[i].mNumIndices; j++)
                srcIndices.push_back(m->mFaces[i].mIndices[j]);
        }

        if (!calLODs)
            outLods.push_back(srcIndices);
        else
            ProcessLods(srcIndices, srcVertices, outLods);

        CIAO_CORE_INFO("Calculated LOD count: {}.", (unsigned)outLods.size());

        // 按 Lod 层次加载索引
        uint32_t numIndices = 0;
        for (size_t l = 0; l < outLods.size(); l++)
        {
            for (size_t i = 0; i < outLods[l].size(); i++)
                indices.push_back(outLods[l][i]);

            result.lodOffset[l] = numIndices;
            numIndices += (int)outLods[l].size();
        }

        result.lodOffset[outLods.size()] = numIndices;
        result.lodCount = (uint32_t)outLods.size();

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
                .LOD = 0,
                .indexOffset = meshData.meshes[i].indexOffset,
                .vertexOffset = vertexOffset,
                .transformIndex = 0
                });
            vertexOffset += meshData.meshes[i].vertexCount;
        }

        FILE* f = fopen(fileName, "wb");
        fwrite(grid.data(), grid.size(), sizeof(DrawData), f);

        fclose(f);

        CIAO_CORE_TRACE("Saving draw data successfully! It cost {}.", timer.elapsedString());
    }
}