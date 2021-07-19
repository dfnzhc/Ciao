#include "pch.h"
#include "Sphere.h"

namespace Ciao
{
    Sphere::Sphere()
    {
    }

    Sphere::~Sphere()
    {
    }

    void Sphere::Init(std::string textureDirectory, int slicesIn, int stacksIn)
    {
        // check if filename passed in -- if so, load texture
        m_Texture.Load(textureDirectory);

        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        m_Texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        m_VBO.Create();

    	m_SlicesIn = slicesIn;
    	m_StacksIn = stacksIn;
    }

    void Sphere::Create()
    {
        m_VBO.Bind();

		// Compute vertex attributes and store in VBO
		int vertexCount = 0;
		for (int stacks = 0; stacks < m_StacksIn; stacks++) {
			float phi = (stacks / (float) (m_StacksIn - 1)) * M_PI;
			for (int slices = 0; slices <= m_SlicesIn; slices++) {
				float theta = (slices / (float) m_SlicesIn) * 2 * M_PI;
				
				glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
				glm::vec2 t = glm::vec2(slices / (float) m_SlicesIn, stacks / (float) m_StacksIn);
				glm::vec3 n = v;

				m_VBO.AddVertexData(&v, sizeof(glm::vec3));
				m_VBO.AddVertexData(&t, sizeof(glm::vec2));
				m_VBO.AddVertexData(&n, sizeof(glm::vec3));

				vertexCount++;
			}
		}

		// Compute indices and store in VBO
		m_NumTriangles = 0;
		for (int stacks = 0; stacks < m_StacksIn; stacks++) {
			for (int slices = 0; slices < m_SlicesIn; slices++) {
				unsigned int nextSlice = slices + 1;
				unsigned int nextStack = (stacks + 1) % m_StacksIn;

				unsigned int index0 = stacks * (m_SlicesIn+1) + slices;
				unsigned int index1 = nextStack * (m_SlicesIn+1) + slices;
				unsigned int index2 = stacks * (m_SlicesIn+1) + nextSlice;
				unsigned int index3 = nextStack * (m_SlicesIn+1) + nextSlice;

				m_VBO.AddIndexData(&index0, sizeof(unsigned int));
				m_VBO.AddIndexData(&index1, sizeof(unsigned int));
				m_VBO.AddIndexData(&index2, sizeof(unsigned int));
				m_NumTriangles++;

				m_VBO.AddIndexData(&index2, sizeof(unsigned int));
				m_VBO.AddIndexData(&index1, sizeof(unsigned int));
				m_VBO.AddIndexData(&index3, sizeof(unsigned int));
				m_NumTriangles++;

			}
		}

		m_VBO.UploadDataToGPU(GL_STATIC_DRAW);

		GLsizei stride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

		// Vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		// Texture coordinates
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
		// Normal vectors
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
    }

    void Sphere::Draw()
    {
    }

    void Sphere::Release()
    {
    }
}
