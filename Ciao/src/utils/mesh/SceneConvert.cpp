#include "pch.h"
#include "SceneConvert.h"

#include "MeshData.h"
#include "SceneGraph.h"

#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include <stb_image_write.h>

#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include "MeshConvert.h"
#include "utils/ReadFile.h"

namespace fs = std::filesystem;

namespace Ciao
{
    static uint32_t ShiftMeshIndices(MeshData& meshData, const std::vector<uint32_t>& meshesToMerge)
	{
		auto minVtxOffset = std::numeric_limits<uint32_t>::max();
		for (auto i : meshesToMerge)
			minVtxOffset = std::min(meshData.meshes[i].vertexOffset, minVtxOffset);

		auto mergeCount = 0u; // calculated by summing index counts in meshesToMerge

		// now shift all the indices in individual index blocks [use minVtxOffset]
		for (auto i : meshesToMerge)
		{
			auto& m = meshData.meshes[i];
			// for how much should we shift the indices in mesh [m]
			const uint32_t delta = m.vertexOffset - minVtxOffset;

			const auto idxCount = m.GetLODIndicesCount(0);
			for (auto ii = 0u; ii < idxCount; ii++)
				meshData.indexData[m.indexOffset + ii] += delta;

			m.vertexOffset = minVtxOffset;

			// sum all the deleted meshes' indices
			mergeCount += idxCount;
		}

		return meshData.indexData.size() - mergeCount;
	}

	// All the meshesToMerge now have the same vertexOffset and individual index values are shifted by appropriate amount
	// Here we move all the indices to appropriate places in the new index array
	static void MergeIndexArray(MeshData& md, const std::vector<uint32_t>& meshesToMerge,
	                            std::map<uint32_t, uint32_t>& oldToNew)
	{
		std::vector<uint32_t> newIndices(md.indexData.size());
		// Two offsets in the new indices array (one begins at the start, the second one after all the copied indices)
		uint32_t copyOffset = 0,
		         mergeOffset = ShiftMeshIndices(md, meshesToMerge);

		const auto mergedMeshIndex = md.meshes.size() - meshesToMerge.size();
		auto newIndex = 0u;
		for (auto midx = 0u; midx < md.meshes.size(); midx++)
		{
			const bool shouldMerge = std::binary_search(meshesToMerge.begin(), meshesToMerge.end(), midx);

			oldToNew[midx] = shouldMerge ? mergedMeshIndex : newIndex;
			newIndex += shouldMerge ? 0 : 1;

			auto& mesh = md.meshes[midx];
			auto idxCount = mesh.GetLODIndicesCount(0);
			// move all indices to the new array at mergeOffset
			const auto start = md.indexData.begin() + mesh.indexOffset;
			mesh.indexOffset = copyOffset;
			const auto offsetPtr = shouldMerge ? &mergeOffset : &copyOffset;
			std::copy(std::execution::par,start, start + idxCount, newIndices.begin() + *offsetPtr);
			*offsetPtr += idxCount;
		}

		md.indexData = newIndices;

		// all the merged indices are now in lastMesh
		Mesh lastMesh = md.meshes[meshesToMerge[0]];
		lastMesh.indexOffset = copyOffset;
		lastMesh.lodOffset[0] = copyOffset;
		lastMesh.lodOffset[1] = mergeOffset;
		lastMesh.lodCount = 1;
		md.meshes.push_back(lastMesh);
	}

	void MergeScene(SceneGraph& scene, MeshData& meshData, const std::string& materialName)
	{
		// 查询材质的索引
		int oldMaterial = (int)std::distance(std::begin(scene.materialNames),
		                                     std::find(std::begin(scene.materialNames), std::end(scene.materialNames),
		                                               materialName));

		std::vector<uint32_t> toDelete;

		for (auto i = 0u; i < scene.hierarchy.size(); i++)
			if (scene.meshes.contains(i) && scene.materialForNode.contains(i) && (scene.materialForNode.at(i) ==
				oldMaterial))
				toDelete.push_back(i);

		std::vector<uint32_t> meshesToMerge(toDelete.size());

		// Convert toDelete indices to mesh indices
		std::transform(std::execution::par,
			toDelete.begin(),
			toDelete.end(),
			meshesToMerge.begin(),
			[&scene](uint32_t i) { return scene.meshes.at(i); });

		// TODO: if merged mesh transforms are non-zero, then we should pre-transform individual mesh vertices in meshData using local transform
		
		// old-to-new mesh indices
		std::map<uint32_t, uint32_t> oldToNew;

		// now move all the meshesToMerge to the end of array
		MergeIndexArray(meshData, meshesToMerge, oldToNew);

		// cutoff all but one of the merged meshes (insert the last saved mesh from meshesToMerge - they are all the same)
		EraseSelected(meshData.meshes, meshesToMerge);

		for (auto& n : scene.meshes)
			n.second = oldToNew[n.second];

		// reattach the node with merged meshes [identity transforms are assumed]
		int newNode = AddNode(scene, 0, 1);
		scene.meshes[newNode] = meshData.meshes.size() - 1;
		scene.materialForNode[newNode] = (uint32_t)oldMaterial;

		DeleteSceneNodes(scene, toDelete);
	}

	std::vector<SceneConfig> readConfigFile(const char* cfgFileName)
	{
		std::ifstream ifs(cfgFileName);
		if (!ifs.is_open())
		{
			CIAO_CORE_ERROR("Failed to load configuration file.");
			exit(EXIT_FAILURE);
		}

		rapidjson::IStreamWrapper isw(ifs);
		rapidjson::Document document;
		const rapidjson::ParseResult parseResult = document.ParseStream(isw);
		assert(!parseResult.IsError());
		assert(document.IsArray());

		std::vector<SceneConfig> configList;

		for (rapidjson::SizeType i = 0; i < document.Size(); i++)
		{
			configList.emplace_back(SceneConfig{
				.fileName = Res(document[i]["input_scene"].GetString()),
				.outputMesh = Res(document[i]["output_mesh"].GetString()),
				.outputScene = Res(document[i]["output_scene"].GetString()),
				.outputMaterials = Res(document[i]["output_materials"].GetString()),
				.scale = (float)document[i]["scale"].GetDouble(),
				.calculateLODs = document[i]["calculate_LODs"].GetBool(),
				.mergeInstances = document[i]["merge_instances"].GetBool()
			});
		}

		return configList;
	}

	void SceneConvert(const std::string& sceneConfigFile)
	{
		fs::create_directory(Res("Textures/ConvertedTex"));

		const auto configs = readConfigFile(sceneConfigFile.c_str());

		for (const auto& cfg : configs)
			ProcessScene(cfg);
	}

	void ProcessScene(const SceneConfig& cfg)
	{
		MeshData meshData;

		uint32_t indexOffset = 0;
		uint32_t vertexOffset = 0;

		// extract base model path
		const std::size_t pathSeparator = cfg.fileName.find_last_of("/\\");
		const std::string basePath = (pathSeparator != std::string::npos)
			                             ? cfg.fileName.substr(0, pathSeparator + 1)
			                             : std::string();

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

		CIAO_CORE_INFO("Loading scene from {}...", cfg.fileName.c_str());

		const aiScene* scene = aiImportFile(cfg.fileName.c_str(), flags);

		if (!scene || !scene->HasMeshes())
		{
			CIAO_CORE_ERROR("Unable to load {}.", cfg.fileName.c_str());
			exit(EXIT_FAILURE);
		}

		// 1. 和 MeshConvert 类似的转换
		meshData.meshes.reserve(scene->mNumMeshes);
		meshData.boxes.reserve(scene->mNumMeshes);

		for (unsigned int i = 0; i != scene->mNumMeshes; i++)
		{
			CIAO_CORE_INFO("Converting meshes {}/{}...", i + 1, scene->mNumMeshes);
			meshData.meshes.push_back(ConvertAIMesh(scene->mMeshes[i], meshData.vertexData, meshData.indexData,
			                                         indexOffset, vertexOffset, cfg.scale, cfg.calculateLODs));
		}

		RecalculateBoundingBox(meshData);
		SaveMeshData(meshData, cfg.outputMesh.c_str());

		// 我们自建的 scence 类
		SceneGraph outScene;
		// 2.加载并转换材质
		std::vector<MaterialDescription> materials;
		std::vector<std::string>& materialNames = outScene.materialNames;

		std::vector<std::string> files;
		std::vector<std::string> opacityMaps;

		for (unsigned int m = 0; m < scene->mNumMaterials; m++)
		{
			aiMaterial* mm = scene->mMaterials[m];

			CIAO_CORE_INFO("Material [{}] {}.", mm->GetName().C_Str(), m);
			materialNames.push_back(std::string(mm->GetName().C_Str()));

			MaterialDescription D = ConvertAIMaterialToDescription(mm, files, opacityMaps);
			materials.push_back(D);
			//dumpMaterial(files, D);
		}

		// 3. 纹理贴图的处理，调整大小和打包
		ConvertAndDownscaleAllTextures(materials, basePath, files, opacityMaps);

		SaveMaterials(cfg.outputMaterials.c_str(), materials, files);

		// 4. 场景的层次结构转换
		Traverse(scene, outScene, scene->mRootNode, -1, 0);

		SaveSceneGraph(cfg.outputScene.c_str(), outScene);
	}

	//--------------------------------------------------------------------------------------
	// 将材质的信息，读取到我们自建的材质描述结构中
	// files 中是所有纹理的路径
	// opacityMaps 中包含了一系列需要和透明贴图组合的纹理
	//--------------------------------------------------------------------------------------
	MaterialDescription ConvertAIMaterialToDescription(const aiMaterial* M, std::vector<std::string>& files,
	                                                   std::vector<std::string>& opacityMaps)
	{
		MaterialDescription D;

		aiColor4D Color;

		// 环境光和自发光都存储到 emissiveColor 中，因为这个项直接与结果相加
		if (aiGetMaterialColor(M, AI_MATKEY_COLOR_AMBIENT, &Color) == AI_SUCCESS)
		{
			D.emission = {Color.r, Color.g, Color.b};
		}
		if (aiGetMaterialColor(M, AI_MATKEY_COLOR_EMISSIVE, &Color) == AI_SUCCESS)
		{
			D.emission.x += Color.r;
			D.emission.y += Color.g;
			D.emission.z += Color.b;
		}

		// diffuse 颜色
		if (aiGetMaterialColor(M, AI_MATKEY_COLOR_DIFFUSE, &Color) == AI_SUCCESS)
		{
			D.baseColor = {Color.r, Color.g, Color.b};
		}

		// 只要透明度大于 0.95 就认为是完全不透明物体
		const float opaquenessThreshold = 0.05f;
		float Opacity = 1.0f;
		if (aiGetMaterialFloat(M, AI_MATKEY_OPACITY, &Opacity) == AI_SUCCESS)
		{
			D.opacity = glm::clamp(1.0f - Opacity, 0.0f, 1.0f);
			if (D.opacity >= 1.0f - opaquenessThreshold) D.opacity = 0.0f;
		}
		if (aiGetMaterialColor(M, AI_MATKEY_COLOR_TRANSPARENT, &Color) == AI_SUCCESS)
		{
			const float Opacity = std::max(std::max(Color.r, Color.g), Color.b);
			D.opacity = glm::clamp(Opacity, 0.0f, 1.0f);
			if (D.opacity >= 1.0f - opaquenessThreshold) D.opacity = 0.0f;
			D.alphaMode = 0.5f;
		}

		// 计算粗糙度和金属度
		float tmp = 1.0f;
		if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &tmp) == AI_SUCCESS)
			D.metallic = tmp;
		if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &tmp) == AI_SUCCESS)
			D.roughness = tmp;

		// 处理纹理
		aiString Path;
		aiTextureMapping Mapping;
		unsigned int UVIndex = 0;
		float Blend = 1.0f;
		aiTextureOp TextureOp = aiTextureOp_Add;
		aiTextureMapMode TextureMapMode[2] = {aiTextureMapMode_Wrap, aiTextureMapMode_Wrap};
		unsigned int TextureFlags = 0;

		if (aiGetMaterialTexture(M, aiTextureType_EMISSIVE, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			D.emissionmapTexID = AddUnique(files, Path.C_Str());
		}

		if (aiGetMaterialTexture(M, aiTextureType_DIFFUSE, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			D.baseColorTexId = AddUnique(files, Path.C_Str());
		}

		// 先看有无法线贴图
		if (aiGetMaterialTexture(M, aiTextureType_NORMALS, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			D.normalmapTexID = AddUnique(files, Path.C_Str());
		}
		// 如果没有法线贴图，查看高度图
		if (D.normalmapTexID == 0xFFFFFFFF)
			if (aiGetMaterialTexture(M, aiTextureType_HEIGHT, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
			                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
				D.normalmapTexID = AddUnique(files, Path.C_Str());

		if (aiGetMaterialTexture(M, aiTextureType_OPACITY, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			D.opacityTexID = AddUnique(opacityMaps, Path.C_Str());
		}

		if (aiGetMaterialTexture(M, aiTextureType_LIGHTMAP, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			D.ambientOcclusionTexID = AddUnique(files, Path.C_Str());
		}

		if (aiGetMaterialTexture(M, aiTextureType_UNKNOWN, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
		                         TextureMapMode, &TextureFlags) == AI_SUCCESS)
		{
			std::string texName{Path.C_Str()};

			if (texName.find("metalRoughness") != std::string::npos)
				D.metallicRoughnessTexID = AddUnique(files, Path.C_Str());
		}


		// 根据纹理的名称，预先设置一些参数
		aiString Name;
		std::string materialName;
		if (aiGetMaterialString(M, AI_MATKEY_NAME, &Name) == AI_SUCCESS)
		{
			materialName = Name.C_Str();
		}
		// apply heuristics
		if ((materialName.find("Glass") != std::string::npos) ||
			(materialName.find("Vespa_Headlight") != std::string::npos))
		{
			D.alphaMode = 0.75f;
			D.opacity = 0.1f;
		}
		else if (materialName.find("Bottle") != std::string::npos)
		{
			D.alphaMode = 0.54f;
			D.opacity = 0.4f;
		}
		else if (materialName.find("Metal") != std::string::npos)
		{
			D.metallic = 1.0f;
			D.roughness = 0.1f;
		}

		return D;
	}

	//--------------------------------------------------------------------------------------
	// 转换并下采样所有纹理贴图
	//--------------------------------------------------------------------------------------
	void ConvertAndDownscaleAllTextures(
		const std::vector<MaterialDescription>& materials, const std::string& basePath, std::vector<std::string>& files,
		std::vector<std::string>& opacityMaps
	)
	{
		std::unordered_map<std::string, uint32_t> opacityMapIndices(files.size());

		for (const auto& m : materials)
			if (m.opacityTexID != 0xFFFFFFFF && m.baseColorTexId != 0xFFFFFFFF)
				opacityMapIndices[files[m.baseColorTexId]] = (uint32_t)m.opacityTexID;

		auto converter = [&](const std::string& s) -> std::string
		{
			return ConvertTexture(s, basePath, opacityMapIndices, opacityMaps);
		};

		std::transform(std::execution::par, std::begin(files), std::end(files), std::begin(files), converter);
	}

	/* find a file in directory which "almost" coincides with the origFile (their lowercase versions coincide) */
	std::string findSubstitute(const std::string& origFile)
	{
		// Make absolute path
		auto apath = fs::absolute(fs::path(origFile));
		// Absolute lowercase filename [we compare with it]
		auto afile = LowercaseString(apath.filename().string());
		// Directory where in which the file should be
		auto dir = fs::path(origFile).remove_filename();

		// Iterate each file non-recursively and compare lowercase absolute path with 'afile'
		for (auto& p : fs::directory_iterator(dir))
			if (afile == LowercaseString(p.path().filename().string()))
				return p.path().string();

		return std::string{};
	}

	std::string fixTextureFile(const std::string& file)
	{
		return fs::exists(file) ? file : findSubstitute(file);
	}

	std::string ConvertTexture(const std::string& file, const std::string& basePath,
	                           std::unordered_map<std::string, uint32_t>& opacityMapIndices,
	                           const std::vector<std::string>& opacityMaps)
	{
		const int maxNewWidth = 512;
		const int maxNewHeight = 512;

		const auto srcFile = ReplaceAll(basePath + file, "\\", "/");
		const auto newFile = std::string(Res("Textures/ConvertedTex/")) + LowercaseString(
			ReplaceAll(ReplaceAll(srcFile, "..", "_"), "/", "_") + std::string("_rescaled")) + std::string(".png");

		// load this image
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(fixTextureFile(srcFile).c_str(), &texWidth, &texHeight, &texChannels,
		                            STBI_rgb_alpha);
		uint8_t* src = pixels;
		texChannels = STBI_rgb_alpha;

		std::vector<uint8_t> tmpImage(maxNewWidth * maxNewHeight * 4);

		if (!src)
		{
			CIAO_CORE_ERROR("Failed to load {} texture.", srcFile.c_str());
			texWidth = maxNewWidth;
			texHeight = maxNewHeight;
			texChannels = STBI_rgb_alpha;
			src = tmpImage.data();
		}
		else
		{
			CIAO_CORE_INFO("Loaded {} {}x{} texture with {} channels.", srcFile.c_str(), texWidth, texHeight,
			               texChannels);
		}

		// 如果存在透明通道贴图，将该贴图的透明度存储到目标纹理中
		if (opacityMapIndices.count(file) > 0)
		{
			const auto opacityMapFile = ReplaceAll(basePath + opacityMaps[opacityMapIndices[file]], "\\", "/");
			int opacityWidth, opacityHeight;
			stbi_uc* opacityPixels = stbi_load(fixTextureFile(opacityMapFile).c_str(), &opacityWidth, &opacityHeight,
			                                   nullptr, 1);

			if (!opacityPixels)
			{
				CIAO_CORE_ERROR("Failed to load opacity mask [{}].", opacityMapFile.c_str());
			}

			assert(opacityPixels);
			assert(texWidth == opacityWidth);
			assert(texHeight == opacityHeight);

			// store the opacity mask in the alpha component of this image
			if (opacityPixels)
				for (int y = 0; y != opacityHeight; y++)
					for (int x = 0; x != opacityWidth; x++)
						src[(y * opacityWidth + x) * texChannels + 3] = opacityPixels[y * opacityWidth + x];

			stbi_image_free(opacityPixels);
		}

		const uint32_t imgSize = texWidth * texHeight * texChannels;
		std::vector<uint8_t> mipData(imgSize);
		uint8_t* dst = mipData.data();

		const int newW = std::min(texWidth, maxNewWidth);
		const int newH = std::min(texHeight, maxNewHeight);

		stbir_resize_uint8(src, texWidth, texHeight, 0, dst, newW, newH, 0, texChannels);

		stbi_write_png(newFile.c_str(), newW, newH, texChannels, dst, 0);

		if (pixels)
			stbi_image_free(pixels);

		return newFile;
	}

	inline void makePrefix(int ofs)
	{
		std::string s = "";
		for (int i = 0; i < ofs; i++)
			s.append("    ");

		CIAO_CORE_INFO("{}", s);
	}

	glm::mat4 toMat4(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		to[0][0] = (float)from.a1; to[0][1] = (float)from.b1; to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
		to[1][0] = (float)from.a2; to[1][1] = (float)from.b2; to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
		to[2][0] = (float)from.a3; to[2][1] = (float)from.b3; to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
		to[3][0] = (float)from.a4; to[3][1] = (float)from.b4; to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;
		return to;
	}

	void Traverse(const aiScene* sourceScene, SceneGraph& scene, aiNode* N, int parent, int ofs)
	{
		// 当前节点
		int newNode = AddNode(scene, parent, ofs);

		if (N->mName.C_Str())
		{
			makePrefix(ofs);
			CIAO_CORE_INFO("Node[{}].name = {}.", newNode, N->mName.C_Str());

			uint32_t stringID = (uint32_t)scene.names.size();
			scene.names.push_back(std::string(N->mName.C_Str()));
			scene.nameForNode[newNode] = stringID;
		}

		// 为当前节点的每一个 mesh 设置一个子节点
		// 名称、mesh、材质索引，并设置本地和全局变换
		for (size_t i = 0; i < N->mNumMeshes; i++)
		{
			int newSubNode = AddNode(scene, newNode, ofs + 1);;

			uint32_t stringID = (uint32_t)scene.names.size();
			scene.names.push_back(std::string(N->mName.C_Str()) + "_Mesh_" + std::to_string(i));
			scene.nameForNode[newSubNode] = stringID;

			int mesh = (int)N->mMeshes[i];
			scene.meshes[newSubNode] = mesh;
			scene.materialForNode[newSubNode] = sourceScene->mMeshes[mesh]->mMaterialIndex;

			makePrefix(ofs);
			CIAO_CORE_INFO("Node[{}].SubNode[{}].mesh      = {}.", newNode, newSubNode, (int)mesh);
			makePrefix(ofs);
			CIAO_CORE_INFO("Node[{}].SubNode[{}].material  = {}.", newNode, newSubNode,
			               sourceScene->mMeshes[mesh]->mMaterialIndex);

			scene.globalTransform[newSubNode] = glm::mat4(1.0f);
			scene.localTransform[newSubNode] = glm::mat4(1.0f);
		}

		// 当前节点的变换
		scene.globalTransform[newNode] = glm::mat4(1.0f);
		scene.localTransform[newNode] = toMat4(N->mTransformation);


		if (N->mParent != nullptr)
		{
			makePrefix(ofs);
			CIAO_CORE_INFO("Node[{}].parent         = {}.", newNode, N->mParent->mName.C_Str());
			makePrefix(ofs);
			CIAO_CORE_INFO("Node[{}].localTransform = -.", newNode);
		}

		// 递归当前节点的子节点
		for (unsigned int n = 0; n < N->mNumChildren; n++)
			Traverse(sourceScene, scene, N->mChildren[n], newNode, ofs + 1);
	}
}
