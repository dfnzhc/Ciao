#include "pch.h"
#include "Skybox.h"

namespace Ciao
{
    const std::vector<std::string> Skybox::Faces{
        "front", "back", "top", "bottom", "right", "left"
    };

    Skybox::Skybox() : m_VAO(0), m_TexUnit(0)
    {
    }

    Skybox::~Skybox()
    {
        Release();
    }

    void Skybox::Init(const std::string& texDir, const std::string& suffix)
    {
        std::vector<std::string> cubeMapTex;
        for (unsigned int i = 0; i < Faces.size(); ++i) {
            cubeMapTex.emplace_back(std::string(texDir + "\\" + Faces[i] + suffix));
        }

        m_CubeMap.Create(cubeMapTex);

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        m_VBO.Create();
        Create();
    }

    void Skybox::Create()
    {
        m_VBO.Bind();

        glm::vec3 vSkyBoxVertices[36] =
        {
            // Front face
            glm::vec3(1, 1, 1), glm::vec3(1, -1, 1), glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1),
            // Back face
            glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, 1, -1), glm::vec3(1, -1, -1),
            // Left face
            glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1), glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1),
            // Right face
            glm::vec3(1, 1, -1), glm::vec3(1, -1, -1), glm::vec3(1, 1, 1), glm::vec3(1, -1, 1),
            // Top face
            glm::vec3(-1, 1, -1), glm::vec3(1, 1, -1), glm::vec3(-1, 1, 1), glm::vec3(1, 1, 1),
            // Bottom face
            glm::vec3(1, -1, -1), glm::vec3(-1, -1, -1), glm::vec3(1, -1, 1), glm::vec3(-1, -1, 1),
        };
        glm::vec2 vSkyBoxTexCoords[4] =
        {
            glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
        };

        glm::vec3 vSkyBoxNormals[6] =
        {
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        };

        for (int i = 0; i < 24; i++) {
            m_VBO.AddData(&vSkyBoxVertices[i], sizeof(glm::vec3));
            m_VBO.AddData(&vSkyBoxTexCoords[i % 4], sizeof(glm::vec2));
            m_VBO.AddData(&vSkyBoxNormals[i / 4], sizeof(glm::vec3));
        }

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
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
    }

    void Skybox::Draw()
    {
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(m_VAO);
        m_CubeMap.Bind(m_TexUnit);
        for (int i = 0; i < 6; i++) {
            glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
        }
        glDepthFunc(GL_LESS);
    }

    void Skybox::Release()
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_CubeMap.Release();
        m_VBO.Release();
    }

    void Skybox::SetTextureUnit(int texUnit)
    {
        m_TexUnit = texUnit;
    }
}
