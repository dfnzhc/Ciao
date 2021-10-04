#pragma once

#include "Utils/UtilsMath.h"

namespace Ciao
{
	class GLBuffer;

	struct VertexData
	{
		vec3 pos;
		vec3 n;
		vec2 tc;
	};


	class GLMesh final
	{
	public:
		explicit GLMesh(const char* fileName);
		GLMesh(const std::vector<uint32_t>& indices, const void* vertexData, uint32_t verticesSize);

		~GLMesh();

		void drawElements(GLenum mode = GL_TRIANGLES) const;
		void drawArrays(GLenum mode, GLint first, GLint count);


		GLuint vao_;
		uint32_t numIndices_;

		std::unique_ptr<GLBuffer> bufferIndices_;
		std::unique_ptr<GLBuffer> bufferVertices_;
	};
}
