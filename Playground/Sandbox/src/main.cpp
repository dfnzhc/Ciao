#include <Ciao.h>
#include <iostream>

using namespace Ciao;

CameraPositioner_Oribit positioner;
Camera camera(positioner);

int main()
{
	App app;

	while (app.beginRender())
	{
		positioner.update();

		if (ImGui::Begin("Control", nullptr))
		{
			ImGui::Text("Hello!");
		}
		ImGui::End();

		
		app.swapBuffers();
	}
}
