#include <Ciao.h>
#include <iostream>

using namespace Ciao;


int main()
{
	App app;

	GLShader vert{Res("Shaders/BaseShader.vert")};
	GLShader frag{Res("Shaders/BaseColor.frag")};
	GLProgram test{ vert, frag };
	

	while (!app.shouldClose())
	{

		app.swapBuffers();
	}
}