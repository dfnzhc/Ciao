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

    const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);
    GLBuffer perFrameDataBuffer(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, kBufferIndex_PerFrameUniforms, perFrameDataBuffer.getHandle(), 0,
                      kUniformBufferSize);
    timer.reset();
    Cubemap cubemap;
    CIAO_TRACE("Load hdr texture costs: {}.", timer.elapsedString());
    
    while (app.beginRender())
    {
        positioner.update();

        const mat4 proj = glm::perspective(45.0f, app.getAspect(), 0.1f, 1000.0f);
        const mat4 view = camera.getViewMatrix();

        PerFrameData perFrameData = {
            .view = view,
            .proj = proj,
        };

        glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, kUniformBufferSize, &perFrameData);

        cubemap.Draw();
        
        if (ImGui::Begin("Control", nullptr))
        {
            ImGui::Text("Hello!");
        }
        ImGui::End();


        app.swapBuffers();
    }
}
