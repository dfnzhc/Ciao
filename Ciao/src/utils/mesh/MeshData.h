#pragma once
#include "MeshUtils.h"

namespace Ciao
{
    constexpr uint32_t kMaxStreams = 8;
    constexpr uint32_t kMaxLODs = 8;

    struct Mesh final
    {
        /* lod 层次的数量. 严格小于 MAX_LODS, 最后一个 LOD 偏移只作为标记 */
        uint32_t lodCount = 1;

        /* 顶点数据流的个数 */
        uint32_t streamCount = 0;

        /* 索引的起始偏移位置 */
        uint32_t indexOffset = 0;
        
        /* 顶点的起始偏移位置 */
        uint32_t vertexOffset = 0;

        /* 顶点的总数 */
        uint32_t vertexCount = 0;

        /* LOD 层次数据的偏移量，最后一个偏移量作为标记，用来计算索引总数 */
        uint32_t lodOffset[kMaxLODs] = {0};

        /* 数据流的偏移量，“指向”不同的流 */
        uint32_t streamOffset[kMaxStreams] = {0};

        /* 流数据的信息 */
        uint32_t streamElementSize[kMaxStreams] = {0};

        /* 获取每个 LOD 层级的索引个数 */
        inline uint32_t GetLODIndicesCount(uint32_t lod) const { return lodOffset[lod + 1] - lodOffset[lod]; }
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
        uint32_t LOD;
        uint32_t indexOffset;
        uint32_t vertexOffset;
        uint32_t transformIndex;
    };

    struct MeshData
    {
        std::vector<uint32_t> indexData;
        std::vector<float> vertexData;
        std::vector<Mesh> meshes;
        std::vector<BoundingBox> boxes;
    };

    MeshFileHeader LoadMeshData(const char* fileName, MeshData& out);
    void SaveMeshData(const MeshData& md, const char* fileName);

    void RecalculateBoundingBox(MeshData& m);

    MeshFileHeader MergeMeshData(MeshData& m, const std::vector<MeshData*> mds);
}
