#pragma once
#include "MeshUtils.h"

namespace Ciao
{
    constexpr uint32_t kMaxStreams = 8;
    constexpr uint32_t kMaxLODs = 8;

    struct Mesh final
    {
        /* 索引的起始偏移位置 */
        uint32_t indexOffset = 0;
        
        /* 顶点的起始偏移位置 */
        uint32_t vertexOffset = 0;

        /* 顶点的总数 */
        uint32_t vertexCount = 0;

        /* 索引的总数 */
        uint32_t indexCount = 0;

        /* ... */
    };

    // Mesh 文件的结构
    //----------------------------
    // | 文件头 | 顶点数据 | 索引数据 |
    //----------------------------
    
    // 文件头，描述文件的存储结构
    //---------------------------------------------------------------------
    // | magicNumber | mesh 的个数 | 顶点数据的偏移位置 | 索引数据大小 | 顶点数据大小
    //---------------------------------------------------------------------
    
    struct MeshFileHeader
    {
        /* 用来检测文件的完整性 */
        uint32_t magicNumber;

        /* 这个头之后的 Mesh 数量 */
        uint32_t meshCount;

        /* 在整合的数据中，Mesh 数据在数据块中的起始偏移 */
        uint32_t dataBlockStartOffset;

        /* 索引数据的内存大小 */
        uint32_t indexDataSize;

        /* 顶点数据的内存大小 */
        uint32_t vertexDataSize;

        /* ... */
    };

    struct DrawData
    {
        uint32_t meshIndex;
        uint32_t materialIndex;
        uint32_t indexOffset;
        uint32_t vertexOffset;
    };

    struct MeshData
    {
        std::vector<uint32_t> indexData;
        std::vector<float> vertexData;
        std::vector<Mesh> meshes;
    };

    bool LoadMeshData(const char* fileName, MeshData& out, MeshFileHeader& header);
    void SaveMeshData(const MeshData& md, const char* fileName);
    MeshFileHeader MergeMeshData(MeshData& m, const std::vector<MeshData*> mds);
}
