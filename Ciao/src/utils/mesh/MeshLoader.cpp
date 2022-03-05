#include "pch.h"
#include "MeshLoader.h"

#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

#include <stb_image_write.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>

#include "utils/ReadFile.h"

namespace Ciao
{
    void LoadMeshFromFile(MeshData& meshData, const MeshConfig& mesh_config)
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
        
        std::string fileName = Res(mesh_config.source);

        const aiScene* scene = aiImportFile(fileName.c_str(), flags);

        if (!scene || !scene->HasMeshes())
        {
            CIAO_CORE_ERROR("Unable to load '{}'", fileName);
            exit(EXIT_FAILURE);
        }

        std::string new_path = Res(mesh_config.meshDataPath);
        if (!std::filesystem::exists(new_path))
            std::filesystem::create_directory(new_path);

        std::string new_TexPath = new_path + "Textures/";
        if (!std::filesystem::exists(new_TexPath))
            std::filesystem::create_directory(new_TexPath);
        
        meshData.meshes.reserve(scene->mNumMeshes);

        uint32_t indexOffset = 0;
        uint32_t vertexOffset = 0;
        for (unsigned int i = 0; i != scene->mNumMeshes; i++)
        {
            CIAO_INFO("Loading meshes {}/{}...", i + 1, scene->mNumMeshes);
            meshData.meshes.push_back(LoadAIMesh(scene->mMeshes[i], meshData.vertexData, meshData.indexData,
                indexOffset, vertexOffset));
        }
        
        SaveMeshData(meshData, Res(mesh_config.meshData));

        std::vector<MaterialDesc> materials;

        std::vector<std::string> files;
        std::vector<std::string> opacityMaps;

        for (unsigned int m = 0; m < scene->mNumMaterials; m++)
        {
            aiMaterial* mm = scene->mMaterials[m];

            CIAO_CORE_INFO("Material [{}] {}.", mm->GetName().C_Str(), m);

            MaterialDesc D = LoadAIMaterial(mm, files, opacityMaps);
            materials.push_back(D);
        }
        

        DownSamplingTextures(
            materials,
            fileName.substr(0, fileName.find_last_of("/") + 1),
            new_TexPath,
            files,
            opacityMaps);

        SaveMaterials(Res(mesh_config.materialData), materials, files);

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

    MaterialDesc LoadAIMaterial(const aiMaterial* M, std::vector<std::string>& texFiles, std::vector<std::string>& OpacityTexFiles)
    {
        MaterialDesc mdesc;

        aiColor4D Color;
        // 环境光和自发光都存储到 emissiveColor 中，因为这个项直接与结果相加
        if (aiGetMaterialColor(M, AI_MATKEY_COLOR_AMBIENT, &Color) == AI_SUCCESS)
        {
            mdesc.emission = {Color.r, Color.g, Color.b};
        }
        if (aiGetMaterialColor(M, AI_MATKEY_COLOR_EMISSIVE, &Color) == AI_SUCCESS)
        {
            mdesc.emission.x += Color.r;
            mdesc.emission.y += Color.g;
            mdesc.emission.z += Color.b;
        }

        // diffuse 颜色
        if (aiGetMaterialColor(M, AI_MATKEY_COLOR_DIFFUSE, &Color) == AI_SUCCESS)
        {
            mdesc.baseColor = {Color.r, Color.g, Color.b};
        }
        // 只要透明度大于 0.95 就认为是完全不透明物体
        float Opacity = 1.0f;
        if (aiGetMaterialFloat(M, AI_MATKEY_OPACITY, &Opacity) == AI_SUCCESS)
        {
            mdesc.opacity = glm::clamp(Opacity, 0.0f, 1.0f);
        }
        if (aiGetMaterialColor(M, AI_MATKEY_COLOR_TRANSPARENT, &Color) == AI_SUCCESS)
        {
            Opacity = std::max(std::max(Color.r, Color.g), Color.b);
            mdesc.opacity = std::max(mdesc.opacity, glm::clamp(1.0f - Opacity, 0.0f, 0.95f));
        }

        // 计算粗糙度和金属度
        float tmp = 1.0f;
        if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &tmp) == AI_SUCCESS)
            mdesc.metallic = tmp;
        if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &tmp) == AI_SUCCESS)
            mdesc.roughness = tmp;
        // if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_ALPHAMODE, &tmp) == AI_SUCCESS)
        //     mdesc.alphaMode = tmp;
        // if (aiGetMaterialFloat(M, AI_MATKEY_GLTF_ALPHACUTOFF, &tmp) == AI_SUCCESS)
        //     mdesc.alphaCutoff = tmp;
        

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
            mdesc.emissionTexID = AddUnique(texFiles, Path.C_Str());
        }
        if (aiGetMaterialTexture(M, aiTextureType_DIFFUSE, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
        {
            mdesc.albedoTexId = AddUnique(texFiles, Path.C_Str());
        }

        // 先看有无法线贴图
        if (aiGetMaterialTexture(M, aiTextureType_NORMALS, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
        {
            mdesc.normalTexID = AddUnique(texFiles, Path.C_Str());
        }
        // 如果没有法线贴图，查看高度图
        if (mdesc.normalTexID == 0xFFFFFFFF)
            if (aiGetMaterialTexture(M, aiTextureType_HEIGHT, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                     TextureMapMode, &TextureFlags) == AI_SUCCESS)
                                         mdesc.normalTexID = AddUnique(texFiles, Path.C_Str());

        if (aiGetMaterialTexture(M, aiTextureType_OPACITY, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
        {
            mdesc.opacityTexID = AddUnique(OpacityTexFiles, Path.C_Str());
        }

        if (aiGetMaterialTexture(M, aiTextureType_LIGHTMAP, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
        {
            mdesc.ambientOcclusionTexID = AddUnique(texFiles, Path.C_Str());
        }
        if (mdesc.ambientOcclusionTexID == 0xFFFFFFFF)
            if (aiGetMaterialTexture(M, aiTextureType_AMBIENT_OCCLUSION, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
            {
                mdesc.ambientOcclusionTexID = AddUnique(texFiles, Path.C_Str());
            }
        
        int msz = aiGetMaterialTextureCount(M, aiTextureType_UNKNOWN);
        for (int i = 0; i < msz; ++i)
        {
            if (aiGetMaterialTexture(M, aiTextureType_UNKNOWN, 0, &Path, &Mapping, &UVIndex, &Blend, &TextureOp,
                                 TextureMapMode, &TextureFlags) == AI_SUCCESS)
            {
                std::string texName{Path.C_Str()};

                // 对应 glTF 文件的纹理命名
                if (mdesc.metallicRoughnessTexID == 0xFFFFFFFF && texName.find("metalRoughness") != std::string::npos)
                    mdesc.metallicRoughnessTexID = AddUnique(texFiles, Path.C_Str());

                
                if (mdesc.ambientOcclusionTexID == 0xFFFFFFFF && texName.find("AO") != std::string::npos)
                    mdesc.ambientOcclusionTexID = AddUnique(texFiles, Path.C_Str());
            }
        }
        

        return mdesc;
    }


    /* 查找当前目录下相近的文件名 */
    std::string FindSubstitute(const std::string& origFile)
    {
        // Make absolute path
        auto apath = std::filesystem::absolute(std::filesystem::path(origFile));
        // Absolute lowercase filename [we compare with it]
        auto afile = LowercaseString(apath.filename().string());
        // Directory where in which the file should be
        auto dir = std::filesystem::path(origFile).remove_filename();

        // Iterate each file non-recursively and compare lowercase absolute path with 'afile'
        for (auto& p : std::filesystem::directory_iterator(dir))
            if (afile == LowercaseString(p.path().filename().string()))
                return p.path().string();

        return std::string{};
    }

    std::string FixTextureFile(const std::string& file)
    {
        return std::filesystem::exists(file) ? file : FindSubstitute(file);
    }
    
    std::string ConvertTexture(const std::string& file, const std::string& path, const std::string& new_path,
                                   std::unordered_map<std::string, uint32_t>& opacityTexIndices,
                                   const std::vector<std::string>& opacityTexs)
    {
        constexpr int maxNewWidth = 512;
        constexpr int maxNewHeight = 512;

        const auto srcFile = ReplaceAll(path + file, "\\", "/");
        const auto newFile = new_path + LowercaseString(GetRawName(srcFile) + std::string("_rescaled")) + std::string(".png");

        // load this image
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(FixTextureFile(srcFile).c_str(), &texWidth, &texHeight, &texChannels,
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

        // 如果存在透明贴图，将该贴图存储到目标纹理的透明通道中
        if (opacityTexIndices.count(file) > 0)
        {
        	const auto opacityMapFile = ReplaceAll(path + opacityTexs[opacityTexIndices[file]], "\\", "/");
        	int opacityWidth, opacityHeight;
        	stbi_uc* opacityPixels = stbi_load(FixTextureFile(opacityMapFile).c_str(), &opacityWidth, &opacityHeight,
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

        // 调整纹理大小
        stbir_resize_uint8(src, texWidth, texHeight, 0, dst, newW, newH, 0, texChannels);

        stbi_write_png(newFile.c_str(), newW, newH, texChannels, dst, 0);

        if (pixels)
        	stbi_image_free(pixels);

        return newFile;
    }

    void DownSamplingTextures(const std::vector<MaterialDesc>& materials, const std::string& path, const std::string& new_path,
        std::vector<std::string>& texFiles, std::vector<std::string>& opacityTexs)
    {
        std::unordered_map<std::string, uint32_t> opacityTexIndices(texFiles.size());

        for (const auto& m : materials)
            if (m.opacityTexID != 0xFFFFFFFF && m.albedoTexId != 0xFFFFFFFF)
                opacityTexIndices[texFiles[m.albedoTexId]] = (uint32_t)m.opacityTexID;

        auto converter = [&](const std::string& s) -> std::string
        {
            return ConvertTexture(s, path, new_path, opacityTexIndices, opacityTexs);
        };

        std::transform(std::execution::par, std::begin(texFiles), std::end(texFiles), std::begin(texFiles), converter);
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
