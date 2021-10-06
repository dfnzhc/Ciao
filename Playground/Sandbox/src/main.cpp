#include <Ciao.h>
#include <iostream>


using namespace Ciao;

struct PerFrameData
{
	mat4 view;
	mat4 proj;
	mat4 light = mat4(0.0f); // unused in this demo
	vec4 cameraPos;
};

CameraPositioner_Oribit positioner;
Camera camera(positioner);

bool g_DrawOpaque = true;
bool g_DrawTransparent = true;
bool g_DrawGrid = true;

int main()
{
	App app;

	GLShader sceneVert{Res("Shaders/Scene.vert")};
	GLShader sceneFrag{Res("Shaders/Scene.frag")};
	GLProgram sceneProgram{sceneVert, sceneFrag};

	GLShader sceneFragOIT{Res("Shaders/Scene_oit.frag")};
	GLProgram sceneProgram_oit{sceneVert, sceneFragOIT};

	GLShader fullScreenQuadVert(Res("Shaders/FullSceenQuad.vert"));
	GLShader combineOIT(Res("Shaders/combineOIT.frag"));
	GLProgram progCombineOIT(fullScreenQuadVert, combineOIT);

	const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);
	GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0,
	                  kUniformBufferSize);

	SceneData sceneData{
		Res("Meshes/bistro_all.meshes"), Res("Meshes/bistro_all.scene"),
		Res("Meshes/bistro_all.materials")
	};
	GLMesh mesh(sceneData);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	Grid grid;
	Skybox skybox;

	GLIndirectBuffer meshesOpaque(sceneData.shapes_.size());
	GLIndirectBuffer meshesTransparent(sceneData.shapes_.size());

	auto isTransparent = [&sceneData](const DrawElementsIndirectCommand& c)
	{
		const auto mtlIndex = c.baseInstance_ & 0xffff;
		const auto& mtl = sceneData.materials_[mtlIndex];
		return (mtl.flags_ & sMaterialFlags_Transparent) > 0;
	};

	mesh.bufferIndirect_.selectTo(meshesOpaque, [&isTransparent](const DrawElementsIndirectCommand& c) -> bool
	{
		return !isTransparent(c);
	});
	mesh.bufferIndirect_.selectTo(meshesTransparent, [&isTransparent](const DrawElementsIndirectCommand& c) -> bool
	{
		return isTransparent(c);
	});

	struct TransparentFragment
	{
		float R, G, B, A;
		float Depth;
		uint32_t Next;
	};

	int width, height;
	glfwGetFramebufferSize(app.getWindow(), &width, &height);
	Framebuffer framebuffer(width, height, GL_RGBA8, GL_DEPTH_COMPONENT24);

	const uint32_t kMaxOITFragments = 16 * 1024 * 1024;
	const uint32_t kBufferIndex_TransparencyLists = kBufferIndex_Materials + 1;
	GLBuffer oitAtomicCounter(sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
	GLBuffer oitTransparencyLists(sizeof(TransparentFragment) * kMaxOITFragments, nullptr, GL_DYNAMIC_STORAGE_BIT);
	Texture oitHeads(GL_TEXTURE_2D, width, height, GL_R32UI);
	glBindImageTexture(0, oitHeads.getHandle(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, oitAtomicCounter.getHandle());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, kBufferIndex_TransparencyLists, oitTransparencyLists.getHandle());

	auto clearTransparencyBuffers = [&oitAtomicCounter, &oitHeads]()
	{
		const uint32_t minusOne = 0xFFFFFFFF;
		const uint32_t zero = 0;
		glClearTexImage(oitHeads.getHandle(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &minusOne);
		glNamedBufferSubData(oitAtomicCounter.getHandle(), 0, sizeof(uint32_t), &zero);
	};


	while (app.beginRender())
	{
		positioner.update();

		glClearNamedFramebufferfv(framebuffer.getHandle(), GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		glClearNamedFramebufferfi(framebuffer.getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);

		const mat4 proj = glm::perspective(45.0f, app.getAspect(), 0.1f, 1000.0f);
		const mat4 view = camera.getViewMatrix();

		PerFrameData perFrameData = {
			.view = view,
			.proj = proj,
			.light = mat4(0.0f),
			.cameraPos = glm::vec4(camera.getPosition(), 1.0f),
		};
		
		glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

		clearTransparencyBuffers();

		framebuffer.bind();
		glEnable(GL_DEPTH_TEST);

		if (g_DrawOpaque)
		{
			sceneProgram.useProgram();
			mesh.draw(sceneData.shapes_.size(), &meshesOpaque);
		}
		
		skybox.draw();

		if (g_DrawGrid)
		{
			grid.draw();
		}

		if (g_DrawTransparent)
		{
			glDepthMask(GL_FALSE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			sceneProgram_oit.useProgram();
			mesh.draw(meshesTransparent.drawCommands_.size(), &meshesTransparent);
			glFlush();
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			glDepthMask(GL_TRUE);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
		framebuffer.unbind();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		progCombineOIT.useProgram();
		glBindTextureUnit(0, framebuffer.getTextureColor().getHandle());
		glDrawArrays(GL_TRIANGLES, 0, 6);


		if (ImGui::Begin("Control", nullptr))
		{
			ImGui::Text("Draw:");
			ImGui::Checkbox("Opaque meshes", &g_DrawOpaque);
			ImGui::Checkbox("Transparent meshes", &g_DrawTransparent);
			ImGui::Checkbox("Grid", &g_DrawGrid);
		}
		ImGui::End();

		app.swapBuffers();
	}
}
