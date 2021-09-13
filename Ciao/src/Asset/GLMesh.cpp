#include "pch.h"
#include "GLMesh.h"

#include "Scene/VtxData.h"

namespace Ciao
{
    struct DrawElementsIndirectCommand
    {
        GLuint count_;
        GLuint instanceCount_;
        GLuint firstIndex_;
        GLuint baseVertex_;
        GLuint baseInstance_;
    };

    GLMesh::GLMesh(const MeshFileHeader& header, const Mesh* meshes, const uint32_t* indices,
                   const float* vertexData)
        : bufferIndices_(header.indexDataSize, indices, 0),
          bufferVertices_(header.vertexDataSize, vertexData, 0),
          bufferIndirect_(sizeof(DrawElementsIndirectCommand) * header.meshCount + sizeof(GLsizei), nullptr,
                          GL_DYNAMIC_STORAGE_BIT),
          numCommands_((GLsizei)header.meshCount)
    {
        glCreateVertexArrays(1, &vao);
        glVertexArrayElementBuffer(vao, bufferIndices_.getHandle());
        glVertexArrayVertexBuffer(vao, 0, bufferVertices_.getHandle(), 0,
                                  sizeof(vec3) + sizeof(vec3) + sizeof(glm::vec2));
        // position
        glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);
        // uv
        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
        glVertexArrayAttribBinding(vao, 1, 0);
        // position
        glEnableVertexArrayAttrib(vao, 2);
        glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_TRUE, sizeof(vec3) + sizeof(glm::vec2));
        glVertexArrayAttribBinding(vao, 2, 0);

        std::vector<uint8_t> drawCommands;

        drawCommands.resize(sizeof(DrawElementsIndirectCommand) * numCommands_ + sizeof(GLsizei));

        // 在缓冲区的最开始存储绘制指令的信息（数量）
        memcpy(drawCommands.data(), &numCommands_, sizeof numCommands_);

        DrawElementsIndirectCommand* cmd = std::launder(
            reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof GLsizei));

        // 准备间接指令缓冲区
        for (uint32_t i = 0; i != numCommands_; i++)
        {
            *cmd++ = {
                .count_ = meshes[i].GetLODIndicesCount(0),
                .instanceCount_ = 1,
                .firstIndex_ = meshes[i].indexOffset,
                .baseVertex_ = meshes[i].vertexOffset,
                .baseInstance_ = 0
            };
        }

        glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());
        glBindVertexArray(0);
    }

    GLMesh::~GLMesh()
    {
        glDeleteVertexArrays(1, &vao);
    }


    void GLMesh::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        shader->UseProgram();
        glBindVertexArray(vao);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());
        glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());
        glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), 0, numCommands_, 0);
    }
}
