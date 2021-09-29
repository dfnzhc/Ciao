#include <Ciao.h>

using namespace Ciao;


int main()
{
	App app;


	while (!app.shouldClose())
	{

		app.swapBuffers();
	}
}