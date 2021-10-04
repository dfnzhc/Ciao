#include "pch.h"
#include "Skybox.h"

namespace Ciao
{
	Skybox::Skybox(const char* envMap, const char* envMapIrradiance)
		: envMap_(GL_TEXTURE_CUBE_MAP, envMap), envMapIrradiance_(GL_TEXTURE_CUBE_MAP, envMapIrradiance)
	{
		glCreateVertexArrays(1, &dummyVAO_);
		const GLuint pbrTextures[] = { envMap_.getHandle(), envMapIrradiance_.getHandle(), brdfLUT_.getHandle() };
		// 绑定点从 5 开始
		glBindTextures(5, 3, pbrTextures);
	}

	Skybox::~Skybox()
	{
		glDeleteVertexArrays(1, &dummyVAO_);
	}

	void Skybox::draw()
	{
		progCube_.useProgram();
		glBindTextureUnit(5, envMap_.getHandle());

		glDepthFunc(GL_LEQUAL);
		//glDepthMask(false);
		glBindVertexArray(dummyVAO_);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
		//glDepthMask(true);
	}
}
