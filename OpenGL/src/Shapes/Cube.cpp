#include "pch.h"
#include "Cube.h"

Cube::Cube()
{
}

Cube::~Cube()
{
}

void Cube::Create(std::string textureDirectory)
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
	
	// set vertices and indices
	glm::vec3 vertices[8] =
	{
		glm::vec3(-1.0, -1.0,  1.0),
		glm::vec3( 1.0, -1.0,  1.0),
		glm::vec3( 1.0,  1.0,  1.0),
		glm::vec3(-1.0,  1.0,  1.0),
		glm::vec3(-1.0, -1.0, -1.0),
		glm::vec3( 1.0, -1.0, -1.0),
		glm::vec3( 1.0,  1.0, -1.0),
		glm::vec3(-1.0,  1.0, -1.0)
	};

	glm::vec2 texCoords[6] = 
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
	};

	glm::vec3 normals[6] =
	{
		glm::vec3( 0,  0,  1),
		glm::vec3( 1,  0,  0),
		glm::vec3( 0,  0, -1),
		glm::vec3(-1,  0,  0),
		glm::vec3( 0,  1,  0),
		glm::vec3( 0, -1,  0)
	};

	const int idxCnt = 6 * 2 * 3; // #face * #triangle * #vertex

	unsigned int indices[idxCnt] =
	{
		// front
		0, 1, 2, 2, 3, 0,
		// right
		1, 5, 6, 6, 2, 1,
		// back
		7, 6, 5, 5, 4, 7,
		// left
		4, 0, 3, 3, 7, 4,
		// bottom
		4, 5, 1, 1, 0, 4,
		// top
		3, 2, 6, 6, 7, 3
	};

	m_NumTriangles = 0;
	for (int i = 0; i < idxCnt; i += 6) {
		std::vector<glm::vec3> points(6);
		points[0] = vertices[indices[i + 0]];
		points[1] = vertices[indices[i + 1]];
		points[2] = vertices[indices[i + 2]];
		
		points[3] = vertices[indices[i + 3]];
		points[4] = vertices[indices[i + 4]];
		points[5] = vertices[indices[i + 5]];

		glm::vec3 normal1 = glm::normalize(glm::cross(points[0] - points[1], points[2] - points[1]));
		glm::vec3 normal2 = glm::normalize(glm::cross(points[3] - points[4], points[5] - points[4]));
		glm::vec3 normal = -glm::normalize(normal1 + normal2);

		for (int j = 0; j < 6; j++) {
			m_VBO.AddVertexData(&points[j], sizeof(glm::vec3));
			m_VBO.AddVertexData(&texCoords[j], sizeof(glm::vec2));
			m_VBO.AddVertexData(&normal, sizeof(glm::vec3));
			indices[i + j] = i + j;
		}

		m_NumTriangles += 2;
	}
	m_VBO.AddIndexData(&indices, sizeof(indices));
	

	m_VBO.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2 * sizeof(glm::vec3)+sizeof(glm::vec2);

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

void Cube::Draw()
{
    glBindVertexArray(m_VAO);
    m_Texture.Bind();
    glDrawElements(GL_TRIANGLES, m_NumTriangles * 3, GL_UNSIGNED_INT, 0);
}

void Cube::Release()
{
    m_Texture.Release();
    glDeleteVertexArrays(1, &m_VAO);
    m_VBO.Release();
}
