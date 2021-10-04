#pragma once
#include "GLShader.h"
#include "Utils/Utils.h"

namespace Ciao
{
	class DummyVAO final
	{
	public:
		DummyVAO();
		~DummyVAO();
		

		void bind();

	private:
		GLuint handle_;
	};

	class Grid final
	{
	public:
		Grid();
		~Grid();

		void draw();

	private:
		GLShader gridVertex_ = GLShader{ Res("Shaders/Grid.vert") };
		GLShader gridFragment_ = GLShader{ Res("Shaders/Grid.frag") };
		GLProgram progGrid_ = GLProgram{ gridVertex_, gridFragment_ };
		GLuint vao_;
	};
}
