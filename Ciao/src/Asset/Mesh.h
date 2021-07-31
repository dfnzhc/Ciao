#pragma once
#include <memory>

#include "Asset/Object.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

namespace Ciao
{
    class Texture;
    
    struct Vertex
    {
        glm::vec3 Position  = glm::vec3{0};
        glm::vec2 TexCoords = glm::vec2{0};
        glm::vec3 Normal    = glm::vec3{0};
        glm::vec3 Tangent   = glm::vec3{0};
        glm::vec3 Bitangent = glm::vec3{0};
        
    };
    
    class Mesh : public Object
    {
    public:
        Mesh();
        ~Mesh() override;

        void Init(std::vector<Vertex>&& vertices, std::vector<UINT>&& indices);
        void Create() override;
        void Draw(std::shared_ptr<ShaderProgram> shader) override;
        void Release() override;

        void SetShader(Ref<ShaderProgram> shader);
        
    private:
        std::vector<Vertex> m_Vertices;
        std::vector<UINT> m_Indices;

        Ref<ShaderProgram> m_Shader;

        GLuint m_VAO;
        VertexBufferObjectIndexed m_VEBO;
    };
}


