#include "pch.h"
#include "GLScene.h"

#include "Utils/Scene/Material.h"
#include "Texture.h"

namespace Ciao
{
    static uint64_t getTextureHandleBindless(uint64_t idx, const std::vector<Texture>& textures)
    {
        if (idx == INVALID_TEXTURE) return 0;

        return textures[idx].getHandleBindless();
    }

    SceneData::SceneData(
        const char* meshFile,
        const char* sceneFile,
        const char* materialFile)
    {
        header_ = loadMeshData(GetAssetDir().append(meshFile).c_str(), meshData_);
        LoadScene(GetAssetDir().append(sceneFile).c_str());

        std::vector<std::string> textureFiles;
        loadMaterials(GetAssetDir().append(materialFile).c_str(), materials_, textureFiles);

        for (const auto& f : textureFiles)
        {
            allMaterialTextures_.emplace_back(GL_TEXTURE_2D, f.c_str());
        }

        for (auto& mtl : materials_)
        {
            mtl.ambientOcclusionMap_ = getTextureHandleBindless(mtl.ambientOcclusionMap_, allMaterialTextures_);
            mtl.emissiveMap_ = getTextureHandleBindless(mtl.emissiveMap_, allMaterialTextures_);
            mtl.albedoMap_ = getTextureHandleBindless(mtl.albedoMap_, allMaterialTextures_);
            mtl.metallicRoughnessMap_ = getTextureHandleBindless(mtl.metallicRoughnessMap_, allMaterialTextures_);
            mtl.normalMap_ = getTextureHandleBindless(mtl.normalMap_, allMaterialTextures_);
        }
    }

    void SceneData::LoadScene(const char* sceneFile)
    {
        Ciao::loadScene(sceneFile, scene_);

        // prepare draw data buffer
        for (const auto& c : scene_.meshes_)
        {
            auto material = scene_.materialForNode_.find(c.first);
            if (material != scene_.materialForNode_.end())
            {
                shapes_.push_back(
                    DrawData{
                        .meshIndex = c.second,
                        .materialIndex = material->second,
                        .LOD = 0,
                        .indexOffset = meshData_.meshes_[c.second].indexOffset,
                        .vertexOffset = meshData_.meshes_[c.second].vertexOffset,
                        .transformIndex = c.first
                    });
            }
        }

        // force recalculation of all global transformations
        markAsChanged(scene_, 0);
        recalculateGlobalTransforms(scene_);
    }

    GLScene::GLScene(const SceneData& data) : numIndices_(data.header_.indexDataSize / sizeof(uint32_t)),
                                              bufferIndices_(data.header_.indexDataSize, data.meshData_.indexData_.data(), 0),
                                              bufferVertices_(data.header_.vertexDataSize, data.meshData_.vertexData_.data(), 0),
                                              bufferMaterials_(sizeof(MaterialDescription) * data.materials_.size(), data.materials_.data(), 0),
                                              bufferIndirect_(sizeof(DrawElementsIndirectCommand) * data.shapes_.size() + sizeof(GLsizei), nullptr,
                                                              GL_DYNAMIC_STORAGE_BIT),
                                              bufferModelMatrices_(sizeof(glm::mat4) * data.shapes_.size(), nullptr, GL_DYNAMIC_STORAGE_BIT),
                                              numCommands_((GLsizei)data.shapes_.size())
    {
        glCreateVertexArrays(1, &vao_);
        glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());
        glVertexArrayVertexBuffer(vao_, 0, bufferVertices_.getHandle(), 0, sizeof(vec3) + sizeof(vec3) + sizeof(vec2));

        // position
        glEnableVertexArrayAttrib(vao_, 0);
        glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao_, 0, 0);
        // uv
        glEnableVertexArrayAttrib(vao_, 1);
        glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
        glVertexArrayAttribBinding(vao_, 1, 0);
        // position
        glEnableVertexArrayAttrib(vao_, 2);
        glVertexArrayAttribFormat(vao_, 2, 3, GL_FLOAT, GL_TRUE, sizeof(vec3) + sizeof(glm::vec2));
        glVertexArrayAttribBinding(vao_, 2, 0);

        std::vector<uint8_t> drawCommands;

        drawCommands.resize(sizeof(DrawElementsIndirectCommand) * numCommands_ + sizeof(GLsizei));

        // 在绘制指令的前面存储绘制的数量
        memcpy(drawCommands.data(), &numCommands_, sizeof(numCommands_));

        DrawElementsIndirectCommand* cmd = std::launder(
            reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof(GLsizei))
        );

        // prepare indirect commands buffer
        for (size_t i = 0; i != numCommands_; i++)
        {
            const uint32_t meshIdx = data.shapes_[i].meshIndex;
            const uint32_t lod = data.shapes_[i].LOD;
            *cmd++ = {
                .count = data.meshData_.meshes_[meshIdx].GetLODIndicesCount(lod),
                .instanceCount = 1,
                .firstIndex = data.shapes_[i].indexOffset,
                .baseVertex = data.shapes_[i].vertexOffset,
                .baseInstance = data.shapes_[i].materialIndex
            };
        }

        glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());

        std::vector<glm::mat4> matrices(data.shapes_.size());
        size_t i = 0;
        for (const auto& c : data.shapes_)
            matrices[i++] = data.scene_.globalTransform_[c.transformIndex];

        glNamedBufferSubData(bufferModelMatrices_.getHandle(), 0, matrices.size() * sizeof(mat4), matrices.data());

    	glBindVertexArray(0);
    }

    GLScene::~GLScene()
    {
        glDeleteVertexArrays(1, &vao_);
    }

    void GLScene::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        shader->UseProgram();
        glBindVertexArray(vao_);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_Materials, bufferMaterials_.getHandle());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_ModelMatrices, bufferModelMatrices_.getHandle());
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());
        glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());
        glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), 0, (GLsizei)numCommands_, 0);
    }
}
