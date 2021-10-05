#include <Ciao.h>
#include <iostream>


using namespace Ciao;

struct PerFrameData
{
	mat4 view;
	mat4 proj;
	vec4 cameraPos;
};

CameraPositioner_Oribit positioner;
Camera camera(positioner);

int main()
{
	App app;
	
	// GLShader gridVert{Res("Shaders/Grid.vert")};
	// GLShader gridFrag{Res("Shaders/Grid.frag")};
	// GLProgram gridProgram{ gridVert, gridFrag };

	GLShader sceneVert{ Res("Shaders/Scene.vert") };
	GLShader sceneFrag{ Res("Shaders/Scene.frag") };
	GLProgram sceneProgram{ sceneVert, sceneFrag };
	
	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);
	
	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer.getHandle(), 0, kUniformBufferSize);

	SceneData sceneData{ Res("Meshes/bistro_all.meshes"), Res("Meshes/bistro_all.scene"),
		Res("Meshes/bistro_all.materials") };
	GLMesh mesh(sceneData);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
	Grid grid;
	Skybox skybox;
	
	
	while (app.beginRender())
	{
		positioner.update();
		
		const mat4 p = glm::perspective(45.0f, app.getAspect(), 0.1f, 1000.0f);
		const mat4 view = camera.getViewMatrix();
		
		const PerFrameData perFrameData = { .view = view, .proj = p, .cameraPos = vec4(camera.getPosition(), 1.0f) };
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		sceneProgram.useProgram();
		mesh.draw(sceneData.shapes_.size());
	
		skybox.draw();
		grid.draw();

		if (ImGui::Begin("Hello")) {
			ImGui::Text("Yu tou!!");

		}
		ImGui::End();
	
		app.swapBuffers();
	}
}