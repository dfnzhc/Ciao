#include "pch.h"
#include "MeshData.h"


namespace Ciao
{
    bool LoadMeshData(const char* fileName, MeshData& out, MeshFileHeader& header)
    {
        Timer timer;

        std::ifstream ifs{fileName, std::ios::in | std::ios::binary};
        if (!ifs)
            return false;

        ifs.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        out.meshes.resize(header.meshCount);
        out.indexData.resize(header.indexDataSize / sizeof(uint32_t));
        out.vertexData.resize(header.vertexDataSize / sizeof(float));
        
        ifs.read(reinterpret_cast<char*>(out.meshes.data()), sizeof(Mesh) * header.meshCount);
        ifs.read(reinterpret_cast<char*>(out.indexData.data()), header.indexDataSize);
        ifs.read(reinterpret_cast<char*>(out.vertexData.data()), header.vertexDataSize);
        
        CIAO_CORE_TRACE("Loading file {} successfully! It costs {}.", fileName, timer.elapsedString());

        return true;
    }

    
    void SaveMeshData(const MeshData& md, const char* fileName)
    {
        Timer timer;
        std::ofstream ofs{fileName, std::ios::out | std::ios::binary};
        
        const MeshFileHeader header = {
            .magicNumber = 0x77777777,
            .meshCount = (uint32_t)md.meshes.size(),
            .dataBlockStartOffset = (uint32_t)(sizeof(MeshFileHeader) + md.meshes.size() * sizeof(Mesh)),
            .indexDataSize = (uint32_t)(md.indexData.size() * sizeof(uint32_t)),
            .vertexDataSize = (uint32_t)(md.vertexData.size() * sizeof(float))
        };

        ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
        ofs.write(reinterpret_cast<const char*>(md.meshes.data()), sizeof(Mesh) * header.meshCount);
        ofs.write(reinterpret_cast<const char*>(md.indexData.data()), header.indexDataSize);
        ofs.write(reinterpret_cast<const char*>(md.vertexData.data()), header.vertexDataSize);
        
        CIAO_CORE_TRACE("Saving file {} successfully! It costs {}.", fileName, timer.elapsedString());
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

            /* 8 ????????????????????????: position, normal + UV */
            uint32_t vtxOffset = totalVertexDataSize / 8;

            // ?????? mesh ?????????????????????
            tbb::parallel_for(0, static_cast<int>(i->meshes.size()), [&m, offs, totalIndexDataSize](int j)
            {
                // m.vertexCount, m.lodCount and m.streamCount ???????????????
                // m.vertexOffset ???????????????, ???????????????????????????????????? (i.e., baked into the indices)
                m.meshes[offs + j].indexOffset += totalIndexDataSize;
            });

            // ??????????????????????????? mesh ??????????????????????????????
            tbb::parallel_for(0, static_cast<int>(i->indexData.size()), [&m, vtxOffset, totalIndexDataSize](int j)
            {
                m.indexData[totalIndexDataSize + j] += vtxOffset;
            });

            // ??????????????? mesh ?????????
            offs += (uint32_t)i->meshes.size();
            
            // ??????????????????????????? 
            totalIndexDataSize += (uint32_t)i->indexData.size();
            
            // ??????????????????????????? 
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
