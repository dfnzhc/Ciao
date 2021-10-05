#pragma once

#include "Utils/UtilsMath.h"

namespace Ciao
{
	constexpr const uint32_t kMaxLODs = 8;
	constexpr const uint32_t kMaxStreams = 8;

	struct Mesh final
	{
		/* Number of LODs in this mesh. Strictly less than MAX_LODS, last LOD offset is used as a marker only */
		/* lod 层次的数量. 严格小于 MAX_LODS, 最后一个 LOD 偏移只作为标记 */
		uint32_t lodCount = 1;

		/* Number of vertex data streams */
		/* 顶点数据流的数量 */
		uint32_t streamCount = 0;

		/* The total count of all previous vertices in this mesh file */
		/* 整个 Mesh 文件中的所有顶点数量 */
		uint32_t indexOffset = 0;

		uint32_t vertexOffset = 0;

		/* Vertex count (for all LODs) */
		/* 顶点的总数（加总所有 LOD 层次） */
		uint32_t vertexCount = 0;

		/* Offsets to LOD data. Last offset is used as a marker to calculate the size */
		/* LOD 层次数据的偏移量，最后一个偏移量只作为标记，用来计算总数 */
		uint32_t lodOffset[kMaxLODs] = {0};

		inline uint32_t GetLODIndicesCount(uint32_t lod) const { return lodOffset[lod + 1] - lodOffset[lod]; }

		/* All the data "pointers" for all the streams */
		/* 数据流的偏移量，“指向”不同的流 */
		uint32_t streamOffset[kMaxStreams] = {0};

		/* Information about stream elemet (size pretty much defines everything else, the "semantics" is defined by the shader) */
		/* 流数据的信息 */
		uint32_t streamElementSize[kMaxStreams] = {0};

		/* Additional information, like mesh name, can be added here */
	};

	struct MeshFileHeader
	{
		/* Unique 64-bit value to check integrity of the file */
		/* 用来检测文件的完整性 */
		uint32_t magicValue;

		/* Number of mesh descriptors following this header */
		/* 这个头之后的 Mesh 数量 */
		uint32_t meshCount;

		/* The offset to combined mesh data (this is the base from which the offsets in individual meshes start) */
		/* 在整合的数据中，Mesh 数据在数据块中的起始偏移 */
		uint32_t dataBlockStartOffset;

		/* How much space index data takes */
		/* 索引数据的内存大小 */
		uint32_t indexDataSize;

		/* How much space vertex data takes */
		/* 顶点数据的内存大小 */
		uint32_t vertexDataSize;

		/* According to your needs, you may add additional metadata fields */
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
		std::vector<uint32_t> indexData_;
		std::vector<float> vertexData_;
		std::vector<Mesh> meshes_;
		std::vector<BoundingBox> boxes_;
	};

	static_assert(sizeof(DrawData) == sizeof(uint32_t) * 6);
	static_assert(sizeof(BoundingBox) == sizeof(float) * 6);

	MeshFileHeader loadMeshData(const char* meshFile, MeshData& out);
	void saveMeshData(const MeshData& m, const char* fileName);

	void RecalculateBoundingBoxes(MeshData& m);

	// Combine a list of meshes to a single mesh container
	// 将一列 Mesh 顶点数据，整合到单个的容器中
	MeshFileHeader MergeMeshData(MeshData& m, const std::vector<MeshData*> md);
}
