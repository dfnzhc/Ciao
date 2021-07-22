#include "pch.h"
#include "Plane.h"

namespace Ciao
{
    Plane::Plane(): m_VAO(0), m_Width(0), m_Height(0), m_TexRepeat(0)
    {
    }

    Plane::~Plane()
    {
        Release();
    }

    void Plane::Init(std::string textureDirectory, float w, float h, float texRepeat)
    {
        m_Width = w;
        m_Height = h;
        m_TexRepeat = texRepeat;

        // Load the texture
        m_Texture.Load(textureDirectory, true);
        // Set parameters for texturing using sampler object
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Use VAO to store state associated with vertices
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Create a VBO
        m_VBO.Create();
        Create();
    }

    void Plane::Create()
    {
        m_VBO.Bind();

        float halfWidth = m_Width / 2.0f;
        float halfHeight = m_Height / 2.0f;

        // Vertex positions
        glm::vec3 planeVertices[4] = 
        {
            glm::vec3(-halfWidth, 0.0f, -halfHeight), 
            glm::vec3(-halfWidth, 0.0f, halfHeight), 
            glm::vec3(halfWidth, 0.0f, -halfHeight), 
            glm::vec3(halfWidth, 0.0f, halfHeight), 
        };

        // Texture coordinates
        glm::vec2 planeTexCoords[4] =
        {
            glm::vec2(0.0f, 0.0f), 
            glm::vec2(0.0f, m_TexRepeat), 
            glm::vec2(m_TexRepeat, 0.0f), 
            glm::vec2(m_TexRepeat, m_TexRepeat)
        };

        // Plane normal
        glm::vec3 planeNormal = glm::vec3(0.0f, 1.0f, 0.0f);

        // Put the vertex attributes in the VBO
        for (unsigned int i = 0; i < 4; i++) {
            m_VBO.AddData(&planeVertices[i], sizeof(glm::vec3));
            m_VBO.AddData(&planeTexCoords[i], sizeof(glm::vec2));
            m_VBO.AddData(&planeNormal, sizeof(glm::vec3));
        }

        // Upload the VBO to the GPU
        m_VBO.UploadDataToGPU(GL_STATIC_DRAW);

        // Set the vertex attribute locations
        GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

        // Vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
        // Texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
        // Normal vectors
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
    }

    void Plane::Draw()
    {
        glBindVertexArray(m_VAO);
        m_Texture.Bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void Plane::Release()
    {
        m_Texture.Release();
        glDeleteVertexArrays(1, &m_VAO);
        m_VBO.Release();
    }
}
