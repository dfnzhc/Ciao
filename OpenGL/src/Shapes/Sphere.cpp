#include "pch.h"
#include "Sphere.h"

Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}

void Sphere::Create(std::string textureDirectory, int slicesIn, int stacksIn)
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
	m_VBO.Bind();
	

	// Compute vertex attributes and store in VBO
	int vertexCount = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		float phi = (stacks / (float) (stacksIn - 1)) * glm::pi<float>();
		for (int slices = 0; slices <= slicesIn; slices++) {
			float theta = (slices / (float) slicesIn) * 2 * glm::pi<float>();
			
			glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
			glm::vec2 t = glm::vec2(slices / (float) slicesIn, stacks / (float) stacksIn);
			glm::vec3 n = v;

			m_VBO.AddVertexData(&v, sizeof(glm::vec3));
			m_VBO.AddVertexData(&t, sizeof(glm::vec2));
			m_VBO.AddVertexData(&n, sizeof(glm::vec3));

			vertexCount++;
		}
	}

	// Compute indices and store in VBO
	m_NumTriangles = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		for (int slices = 0; slices < slicesIn; slices++) {
			unsigned int nextSlice = slices + 1;
			unsigned int nextStack = (stacks + 1) % stacksIn;

			unsigned int index0 = stacks * (slicesIn+1) + slices;
			unsigned int index1 = nextStack * (slicesIn+1) + slices;
			unsigned int index2 = stacks * (slicesIn+1) + nextSlice;
			unsigned int index3 = nextStack * (slicesIn+1) + nextSlice;

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
    glBindVertexArray(m_VAO);
    m_Texture.Bind();
    glDrawElements(GL_TRIANGLES, m_NumTriangles*3, GL_UNSIGNED_INT, 0);
}

void Sphere::Release()
{
    m_Texture.Release();
    glDeleteVertexArrays(1, &m_VAO);
    m_VBO.Release();
}
