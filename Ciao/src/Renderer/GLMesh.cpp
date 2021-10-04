#include "pch.h"
#include "GLMesh.h"

#include "GLShader.h"

namespace Ciao
{
	GLMesh::GLMesh(const char* fileName)
	{
		std::vector<VertexData> vertices;
		std::vector<uint32_t> indices;

		{
			const aiScene* scene = aiImportFile(fileName, aiProcess_Triangulate);
			if (!scene || !scene->HasMeshes())
			{
				CIAO_CORE_ERROR("Unable to load '{}'", fileName);
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

	GLMesh::GLMesh(const std::vector<uint32_t>& indices, const void* vertexData, uint32_t verticesSize)
		: numIndices_((uint32_t)indices.size()),
		  bufferIndices_(indices.size()
			                 ? std::make_unique<GLBuffer>(indices.size() * sizeof(uint32_t), indices.data(), 0)
			                 : nullptr),
		  bufferVertices_(std::make_unique<GLBuffer>(verticesSize, vertexData, GL_DYNAMIC_STORAGE_BIT))
	{
		glCreateVertexArrays(1, &vao_);
		if (bufferIndices_)
			glVertexArrayElementBuffer(vao_, bufferIndices_->getHandle());
	}

	GLMesh::~GLMesh()
	{
		glDeleteVertexArrays(1, &vao_);
	}

	void GLMesh::drawElements(GLenum mode) const
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferVertices_->getHandle());
		glDrawElements(mode, static_cast<GLsizei>(numIndices_), GL_UNSIGNED_INT, nullptr);
	}

	void GLMesh::drawArrays(GLenum mode, GLint first, GLint count)
	{
		glBindVertexArray(vao_);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferVertices_->getHandle());
		glDrawArrays(mode, first, count);
	}
}
