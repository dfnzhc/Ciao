#include "pch.h"
#include "MeshObj.h"

namespace Ciao
{
    MeshObj::MeshObj()
    {
    }

    MeshObj::~MeshObj()
    {
        Release();
    }

    void MeshObj::Init(std::vector<Vertex>&& vertices, std::vector<UINT>&& indices)
    {
        m_Vertices = std::move(vertices);
        m_Indices = std::move(indices);

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        m_VEBO.Create();

        Create();
    }

    void MeshObj::Create()
    {
        m_VEBO.Bind();

        m_VEBO.AddVertexData(&m_Vertices[0], sizeof(Vertex) * m_Vertices.size());
        m_VEBO.AddIndexData(&m_Indices[0], sizeof(UINT) * m_Indices.size());

        m_VEBO.UploadDataToGPU(GL_STATIC_DRAW);

        GLsizei stride = sizeof(Vertex);

        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
        // Texture coordinates
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)));
        // Normal vectors
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
        // Tangent vectors
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(glm::vec3)+sizeof(glm::vec2)));
        // Bitangent vectors
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(glm::vec3)+sizeof(glm::vec2)));
    }

    void MeshObj::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        shader->UseProgram();
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }

    void MeshObj::Release()
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VEBO.Release();
    }
}
