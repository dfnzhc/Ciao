#include "pch.h"
#include "VtxData.h"

#include "Utils/Utils.h"

namespace Ciao
{
    MeshFileHeader LoadMeshData(const char* meshFile, MeshData& out)
    {
        MeshFileHeader header;

        FILE* f = fopen(meshFile, "rb");

        CIAO_ASSERT(f, "No Mesh data FOUND!");


        if (fread(&header, 1, sizeof(header), f) != sizeof(header))
        {
            CIAO_CORE_ERROR("Unable to read mesh file.");
            exit(EXIT_FAILURE);
        }

        out.meshes_.resize(header.meshCount);
        if (fread(out.meshes_.data(), sizeof(Mesh), header.meshCount, f) != header.meshCount)
        {
            CIAO_CORE_ERROR("Could not read mesh descriptors.");
            exit(EXIT_FAILURE);
        }

        out.boxes_.resize(header.meshCount);
        if (fread(out.boxes_.data(), sizeof(BoundingBox), header.meshCount, f) != header.meshCount)
        {
            CIAO_CORE_ERROR("Could not read bounding boxes.");
            exit(255);
        }

        out.indexData_.resize(header.indexDataSize / sizeof(uint32_t));
        out.vertexData_.resize(header.vertexDataSize / sizeof(float));

        if ((fread(out.indexData_.data(), 1, header.indexDataSize, f) != header.indexDataSize) ||
            (fread(out.vertexData_.data(), 1, header.vertexDataSize, f) != header.vertexDataSize))
        {
            CIAO_CORE_ERROR("Unable to read index/vertex data.");
            exit(255);
        }

        fclose(f);

        return header;
    }

    void SaveMeshData(const MeshData& m, const char* fileName)
    {
        FILE* f = fopen(fileName, "wb");
        const MeshFileHeader header = {
            .magicValue = 0x12345678,
            .meshCount = (uint32_t)m.meshes_.size(),
            .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + m.meshes_.size() * sizeof(Mesh)),
            .indexDataSize = (uint32_t)(m.indexData_.size() * sizeof(uint32_t)),
            .vertexDataSize = (uint32_t)(m.vertexData_.size() * sizeof(float))
        };

        fwrite(&header, 1, sizeof(header), f);
        fwrite(m.meshes_.data(), sizeof(Mesh), header.meshCount, f);
        fwrite(m.boxes_.data(), sizeof(BoundingBox), header.meshCount, f);
        fwrite(m.indexData_.data(), 1, header.indexDataSize, f);
        fwrite(m.vertexData_.data(), 1, header.vertexDataSize, f);

        fclose(f);
    }

    void SaveBoundingBoxes(const char* fileName, const std::vector<BoundingBox>& boxes)
    {
        FILE* f = fopen(fileName, "wb");

        if (!f)
        {
            CIAO_CORE_ERROR("Error opening bounding boxes file for writing.");
            exit(255);
        }

        const uint32_t sz = (uint32_t)boxes.size();
        fwrite(&sz, 1, sizeof(sz), f);
        fwrite(boxes.data(), sz, sizeof(BoundingBox), f);

        fclose(f);
    }

    void LoadBoundingBoxes(const char* fileName, std::vector<BoundingBox>& boxes)
    {
        FILE* f = fopen(fileName, "rb");

        if (!f)
        {
            CIAO_CORE_ERROR("Error opening bounding boxes file.");
            exit(255);
        }

        uint32_t sz;
        fread(&sz, 1, sizeof(sz), f);

        // TODO: check file size, divide by bounding box size
        boxes.resize(sz);
        fread(boxes.data(), sz, sizeof(BoundingBox), f);

        fclose(f);
    }

    // Combine a list of meshes to a single mesh container
    // 将一列 Mesh 顶点数据，整合到单个的容器中
    MeshFileHeader MergeMeshData(MeshData& m, const std::vector<MeshData*> md)
    {
        uint32_t totalVertexDataSize = 0;
        uint32_t totalIndexDataSize = 0;

        uint32_t offs = 0;

        for (const MeshData* i : md)
        {
            mergeVectors(m.indexData_, i->indexData_);
            mergeVectors(m.vertexData_, i->vertexData_);
            mergeVectors(m.meshes_, i->meshes_);
            mergeVectors(m.boxes_, i->boxes_);

            /* 8 is the number of per-vertex attributes: position, normal + UV */
            /* 8 是顶点属性的大小: position, normal + UV */
            uint32_t vtxOffset = totalVertexDataSize / 8;

            for (size_t j = 0; j < (uint32_t)i->meshes_.size(); j++)
                // m.vertexCount, m.lodCount and m.streamCount 都不会改变
                // m.vertexOffset 也不会改变, 因为顶点的偏移都是局部的 (i.e., baked into the indices)
                m.meshes_[offs + j].indexOffset += totalIndexDataSize;

            // shift individual indices
            for (size_t j = 0; j < i->indexData_.size(); j++)
                m.indexData_[totalIndexDataSize + j] += vtxOffset;

            offs += (uint32_t)i->meshes_.size();
            totalIndexDataSize += (uint32_t)i->indexData_.size();
            totalVertexDataSize += (uint32_t)i->vertexData_.size();
        }

        return MeshFileHeader{
            .magicValue = 0x12345678,
            .meshCount = (uint32_t)offs,
            .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + offs * sizeof(Mesh)),
            .indexDataSize = static_cast<uint32_t>(totalIndexDataSize * sizeof(uint32_t)),
            .vertexDataSize = static_cast<uint32_t>(totalVertexDataSize * sizeof(float))
        };
    }

    void RecalculateBoundingBoxes(MeshData& m)
    {
        m.boxes_.clear();

        for (const auto& mesh : m.meshes_)
        {
            const auto numIndices = mesh.GetLODIndicesCount(0);

            glm::vec3 vmin(std::numeric_limits<float>::max());
            glm::vec3 vmax(std::numeric_limits<float>::lowest());

            for (auto i = 0; i != numIndices; i++)
            {
                auto vtxOffset = m.indexData_[mesh.indexOffset + i] + mesh.vertexOffset;
                const float* vf = &m.vertexData_[vtxOffset * kMaxStreams];
                vmin = glm::min(vmin, vec3(vf[0], vf[1], vf[2]));
                vmax = glm::max(vmax, vec3(vf[0], vf[1], vf[2]));
            }

            m.boxes_.emplace_back(vmin, vmax);
        }
    }
}
