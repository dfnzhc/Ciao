#include <Ciao.h>
#include <iostream>
#include <tbb/tbb.h>

using namespace Ciao;

CameraPositioner_Oribit positioner;
Camera camera(positioner);

struct PerFrameData
{
    mat4 view;
    mat4 proj;
};

int main()
{
    App app;
    Timer timer;

    GLShader shaderVertex  (Res("Shaders/GL03_mesh_inst.vert"));
    GLShader shaderGeom  (Res("Shaders/GL03_mesh_inst.geom"));
    GLShader shaderFragment(Res("Shaders/GL03_mesh_inst.frag"));
    GLProgram program(shaderVertex, shaderGeom, shaderFragment);

    const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);
    GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0,
                      kUniformBufferSize);
    timer.reset();
    Cubemap cubemap;
    CIAO_TRACE("Load hdr texture costs: {}.", timer.elapsedString());

    Scene scene = ReadSceneFile(Res("Scenes/scene.json"));
    //scene.LoadMeshes();
    MeshData mesh;
    MeshFileHeader header;
    //LoadMeshFromFile(mesh, Res(scene.meshConfigs[0].source.c_str()));
    //SaveMeshData(mesh, Res(scene.meshConfigs[0].meshDataPath.c_str()));
    LoadMeshData(Res(scene.meshConfigs[0].meshDataPath), mesh, header);

    GLMesh me{header, mesh.meshes.data(), mesh.indexData.data(), mesh.vertexData.data()};

    const mat4 m(1.0f);
    GLBuffer modelMatrices(sizeof(mat4), value_ptr(m), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, modelMatrices.getHandle());
    
    while (app.beginRender())
    {
        positioner.update();

        const mat4 proj = glm::perspective(glm::radians(45.0f), app.getAspect(), 0.1f, 1000.0f);
        const mat4 view = camera.getViewMatrix();

        PerFrameData perFrameData = {
            .view = view,
            .proj = proj,
        };

        glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

        glEnable(GL_DEPTH_TEST);
        program.useProgram();
        me.draw(header);

        cubemap.Draw();
        
        if (ImGui::Begin("Control", nullptr))
        {
            ImGui::Text("Hello!");
        }
        ImGui::End();


        app.swapBuffers();
    }
}
