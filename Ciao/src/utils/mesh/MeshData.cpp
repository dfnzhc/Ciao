#include "pch.h"
#include "MeshData.h"


namespace Ciao
{
    MeshFileHeader LoadMeshData(const char* fileName, MeshData& out)
    {
        Timer timer;
        MeshFileHeader header;

        FILE* f = fopen(fileName, "rb");

        if (!f)
        {
            CIAO_CORE_ERROR("No such file: {}.", fileName);
            CIAO_ASSERT(f, "Load mesh data file FAILED.");
        }

        if (fread(&header, 1, sizeof(header), f) != sizeof(header))
        {
            CIAO_CORE_ERROR("Can not read mesh in file: {}.", fileName);
            exit(EXIT_FAILURE);
        }

        out.meshes.resize(header.meshCount);
        if (fread(out.meshes.data(), sizeof(Mesh), header.meshCount, f) != header.meshCount)
        {
            CIAO_CORE_ERROR("Can not read mesh descriptors in file: {}.", fileName);
            exit(EXIT_FAILURE);
        }

        out.boxes.resize(header.meshCount);
        if (fread(out.boxes.data(), sizeof(BoundingBox), header.meshCount, f) != header.meshCount)
        {
            CIAO_CORE_ERROR("Can not read bounding boxes in file: {}.", fileName);
            exit(EXIT_FAILURE);
        }

        out.indexData.resize(header.indexDataSize / sizeof(uint32_t));
        out.vertexData.resize(header.vertexDataSize / sizeof(float));

        if ((fread(out.indexData.data(), 1, header.indexDataSize, f) != header.indexDataSize) ||
            (fread(out.vertexData.data(), 1, header.vertexDataSize, f) != header.vertexDataSize))
        {
            CIAO_CORE_ERROR("Can not read index/vertex data in file: {}.", fileName);
            exit(EXIT_FAILURE);
        }

        fclose(f);
        
        CIAO_CORE_TRACE("Loading file {} successfully! It costs {}.", fileName, timer.elapsedString());

        return header;
    }

    
    void SaveMeshData(const MeshData& md, const char* fileName)
    {
        Timer timer;
        
        FILE* f = fopen(fileName, "wb");
        const MeshFileHeader header = {
            .magicNumber = 0x77777777,
            .meshCount = (uint32_t)md.meshes.size(),
            .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + md.meshes.size() * sizeof(Mesh)),
            .indexDataSize = (uint32_t)(md.indexData.size() * sizeof(uint32_t)),
            .vertexDataSize = (uint32_t)(md.vertexData.size() * sizeof(float))
        };

        fwrite(&header, 1, sizeof(header), f);
        fwrite(md.meshes.data(), sizeof(Mesh), header.meshCount, f);
        fwrite(md.boxes.data(), sizeof(BoundingBox), header.meshCount, f);
        fwrite(md.indexData.data(), 1, header.indexDataSize, f);
        fwrite(md.vertexData.data(), 1, header.vertexDataSize, f);

        fclose(f);
        
        CIAO_CORE_TRACE("Saving file {} successfully! It costs {}.", fileName, timer.elapsedString());
    }

    void RecalculateBoundingBox(MeshData& m)
    {
        
    }

    MeshFileHeader MergeMeshData(MeshData& m, const std::vector<MeshData*> mds)
    {
        Timer timer;
        
        uint32_t totalVertexDataSize = 0;
        uint32_t totalIndexDataSize = 0;

        uint32_t offs = 0;
        for (const MeshData* i : mds)
        {
            MergeVectors(m.indexData, i->indexData);
            MergeVectors(m.vertexData, i->vertexData);
            MergeVectors(m.meshes, i->meshes);
            MergeVectors(m.boxes, i->boxes);

            /* 8 是顶点属性的大小: position, normal + UV */
            uint32_t vtxOffset = totalVertexDataSize / 8;

            // 更新 mesh 的索引偏移位置
            tbb::parallel_for(0, static_cast<int>(i->meshes.size()), [&m, offs, totalIndexDataSize](int j)
            {
                // m.vertexCount, m.lodCount and m.streamCount 都不会改变
                // m.vertexOffset 也不会改变, 因为顶点的偏移都是局部的 (i.e., baked into the indices)
                m.meshes[offs + j].indexOffset += totalIndexDataSize;
            });

            // 根据顶点的偏移，对 mesh 的索引进行一定的偏移
            tbb::parallel_for(0, static_cast<int>(i->indexData.size()), [&m, vtxOffset, totalIndexDataSize](int j)
            {
                m.indexData[totalIndexDataSize + j] += vtxOffset;
            });

            // 当前加载的 mesh 的个数
            offs += (uint32_t)i->meshes.size();
            
            // 当前加载的索引个数 
            totalIndexDataSize += (uint32_t)i->indexData.size();
            
            // 当前加载的顶点个数 
            totalVertexDataSize += (uint32_t)i->vertexData.size();
        }
        
        CIAO_CORE_TRACE("Merging mesh data successfully! It costs {}.", timer.elapsedString());
        
        return MeshFileHeader{
            .magicNumber = 0x77777777,
            .meshCount = offs,
            .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + offs * sizeof(Mesh)),
            .indexDataSize = static_cast<uint32_t>(totalIndexDataSize * sizeof(uint32_t)),
            .vertexDataSize = static_cast<uint32_t>(totalVertexDataSize * sizeof(float))
        };
    }
}
