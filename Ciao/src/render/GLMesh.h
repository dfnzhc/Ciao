#pragma once
#include "GLBuffer.h"
#include "utils/mesh/MeshData.h"

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

    class GLMesh
    {
    public:
        GLMesh(const MeshFileHeader& header, const Mesh* meshes, const uint32_t* indices, const float* vertexData)
            : numIndices_(header.indexDataSize / sizeof(uint32_t))
              , bufferIndices_(header.indexDataSize, indices, 0)
              , bufferVertices_(header.vertexDataSize, vertexData, 0)
              , bufferIndirect_(sizeof(DrawElementsIndirectCommand) * header.meshCount + sizeof(GLsizei), nullptr, GL_DYNAMIC_STORAGE_BIT)
        {
            glCreateVertexArrays(1, &vao_);
            glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());
            glVertexArrayVertexBuffer(vao_, 0, bufferVertices_.getHandle(), 0,
                                      sizeof(vec3) + sizeof(vec3) + sizeof(vec2));
            // position
            glEnableVertexArrayAttrib(vao_, 0);
            glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribBinding(vao_, 0, 0);
            // uv
            glEnableVertexArrayAttrib(vao_, 1);
            glVertexArrayAttribFormat(vao_, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
            glVertexArrayAttribBinding(vao_, 1, 0);
            // normal
            glEnableVertexArrayAttrib(vao_, 2);
            glVertexArrayAttribFormat(vao_, 2, 3, GL_FLOAT, GL_TRUE, sizeof(vec3) + sizeof(vec2));
            glVertexArrayAttribBinding(vao_, 2, 0);

            std::vector<uint8_t> drawCommands;

            const GLsizei numCommands = (GLsizei)header.meshCount;

            drawCommands.resize(sizeof(DrawElementsIndirectCommand) * numCommands + sizeof(GLsizei));

            // store the number of draw commands in the very beginning of the buffer
            memcpy(drawCommands.data(), &numCommands, sizeof(numCommands));

            DrawElementsIndirectCommand* cmd = std::launder(
                reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof(GLsizei))
            );

            // prepare indirect commands buffer
            for (uint32_t i = 0; i != numCommands; i++)
            {
                *cmd++ = {
                    .count_ = meshes[i].indexCount,
                    .instanceCount_ = 1,
                    .firstIndex_ = meshes[i].indexOffset,
                    .baseVertex_ = meshes[i].vertexOffset,
                    .baseInstance_ = 0
                };
            }

            glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());
            glBindVertexArray(vao_);
        }

        void draw(const MeshFileHeader& header) const
        {
            glBindVertexArray(vao_);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());
            glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());
            glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), 0,
                                             (GLsizei)header.meshCount, 0);
        }

        ~GLMesh()
        {
            glDeleteVertexArrays(1, &vao_);
        }

        GLMesh(const GLMesh&) = delete;
        GLMesh(GLMesh&&) = default;
    private:
        GLuint vao_;
        uint32_t numIndices_;

        GLBuffer bufferIndices_;
        GLBuffer bufferVertices_;
        GLBuffer bufferIndirect_;
    };
}
