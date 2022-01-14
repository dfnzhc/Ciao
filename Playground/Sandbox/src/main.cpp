#include <Ciao.h>
#include <iostream>

using namespace Ciao;


int main()
{
	App app;
	app.Init();
	
	
	CameraPositioner_Oribit positioner;
	Camera camera(positioner);

	while (app.isRunning())
	{
		app.mainLoop();
		positioner.update();

		CIAO_TRACE("{}, {}", camera.getPosition().x, camera.getPosition().y);
	}
}
