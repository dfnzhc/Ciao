#pragma once
#include "GLShader.h"
#include "Texture.h"
#include "Utils/Utils.h"

namespace Ciao
{
	class Skybox
	{
	public:
		Skybox( const char* envMap = Res("Textures/immenstadter_horn_2k.hdr"), 
				const char* envMapIrradiance = Res("Textures/immenstadter_horn_2k_irradiance.hdr"));

		~Skybox();

		void draw();

	private:
		Texture envMap_;
		Texture envMapIrradiance_;
		Texture brdfLUT_ = { GL_TEXTURE_2D, Res("Textures/brdfLUT.ktx") };
		GLShader shdCubeVertex_ = GLShader{ Res("Shaders/Skybox.vert") };
		GLShader shdCubeFragment_ = GLShader{ Res("Shaders/Skybox.frag") };
		GLProgram progCube_ = GLProgram{ shdCubeVertex_, shdCubeFragment_ };
		GLuint dummyVAO_;
	};
}
