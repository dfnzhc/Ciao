#pragma once
#include "Renderer/Shader.h"

namespace Ciao
{
    struct Mesh;
    struct MeshFileHeader;

    class GLMesh
    {
    public:
        GLMesh(const MeshFileHeader& header, const Mesh* meshes, const uint32_t* indices, const float* vertexData);
        ~GLMesh();
        GLMesh(const GLMesh&) = delete;
        GLMesh(GLMesh&&) = default;
        
        void Draw(std::shared_ptr<ShaderProgram> shader);
        
    private:
        GLuint vao;
        uint32_t numIndices_;
        GLsizei numCommands_;

        GLBuffer bufferIndices_;
        GLBuffer bufferVertices_;
        GLBuffer bufferIndirect_;
    };
}
