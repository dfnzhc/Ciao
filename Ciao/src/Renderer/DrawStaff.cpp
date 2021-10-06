#include "pch.h"
#include "DrawStaff.h"

namespace Ciao
{
	DummyVAO::DummyVAO()
	{
		glCreateVertexArrays(1, &handle_);
	}

	DummyVAO::~DummyVAO()
	{
		glDeleteVertexArrays(1, &handle_);
	}

	void DummyVAO::bind()
	{
		glBindVertexArray(handle_);
	}

	Grid::Grid()
	{
		glCreateVertexArrays(1, &vao_);
	}

	Grid::~Grid()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	void Grid::draw()
	{
		progGrid_.useProgram();
		glBindVertexArray(vao_);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
		glDisable(GL_BLEND);
	}


	MeshPVP::MeshPVP(const std::vector<uint32_t>& indices, const void* vertexData, uint32_t verticesSize)
		: numIndices_((uint32_t)indices.size())
		, bufferIndices_(indices.size() ? std::make_unique<GLBuffer>(indices.size() * sizeof(uint32_t), indices.data(), 0) : nullptr)
		, bufferVertices_(std::make_unique<GLBuffer>(verticesSize, vertexData, GL_DYNAMIC_STORAGE_BIT))
	{
		glCreateVertexArrays(1, &vao_);
		if (bufferIndices_)
			glVertexArrayElementBuffer(vao_, bufferIndices_->getHandle());
	}

	MeshPVP::MeshPVP(const char* fileName)
	{
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;
		{
			const aiScene* scene = aiImportFile(fileName, aiProcess_Triangulate);
			if (!scene || !scene->HasMeshes())
			{
				printf("Unable to load '%s'\n", fileName);
				exit(255);
			}

			const aiMesh* mesh = scene->mMeshes[0];
			for (unsigned i = 0; i != mesh->mNumVertices; i++)
			{
				const aiVector3D v = mesh->mVertices[i];
				const aiVector3D n = mesh->mNormals[i];
				const aiVector3D t = mesh->mTextureCoords[0][i];
				vertices.push_back({ .pos = vec3(v.x, v.y, v.z), .n = vec3(n.x, n.y, n.z), .tc = vec2(t.x, 1.0f - t.y) });
			}
			for (unsigned i = 0; i != mesh->mNumFaces; i++)
				for (unsigned j = 0; j != 3; j++)
					indices.push_back(mesh->mFaces[i].mIndices[j]);
			aiReleaseImport(scene);
		}
		const size_t kSizeIndices = sizeof(uint32_t) * indices.size();
		const size_t kSizeVertices = sizeof(VertexData) * vertices.size();
		numIndices_ = (uint32_t)indices.size();
		bufferIndices_ = std::make_unique<GLBuffer>((uint32_t)kSizeIndices, indices.data(), 0);
		bufferVertices_ = std::make_unique<GLBuffer>((uint32_t)kSizeVertices, vertices.data(), 0);
		glCreateVertexArrays(1, &vao_);
		glVertexArrayElementBuffer(vao_, bufferIndices_->getHandle());
	}

	void MeshPVP::drawElements(GLenum mode) const
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferVertices_->getHandle());
		glDrawElements(mode, static_cast<GLsizei>(numIndices_), GL_UNSIGNED_INT, nullptr);
	}
	void MeshPVP::drawArrays(GLenum mode, GLint first, GLint count)
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferVertices_->getHandle());
		glDrawArrays(mode, first, count);
	}

	MeshPVP::~MeshPVP()
	{
		glDeleteVertexArrays(1, &vao_);
	}
}
