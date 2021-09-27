#include "pch.h"
#include "Skybox.h"

#include "Shader.h"

namespace Ciao
{
    Skybox::Skybox(const char* envMap, const char* envMapIrradiance) :
        envMap_(GL_TEXTURE_CUBE_MAP, GetAssetDir().append(envMap).c_str()),
        envMapIrradiance_(GL_TEXTURE_CUBE_MAP, GetAssetDir().append(envMapIrradiance).c_str())
    {
        glCreateVertexArrays(1, &dummyVAO_);
        const GLuint pbrTextures[] = {envMap_.getHandle(), envMapIrradiance_.getHandle(), brdfLUT_.getHandle()};

        glBindTextures(5, 3, pbrTextures);
    }

    Skybox::~Skybox()
    {
        glDeleteVertexArrays(1, &dummyVAO_);
    }

    void Skybox::Draw(std::shared_ptr<ShaderProgram> shader)
    {
        if (shader != nullptr)
            shader->UseProgram();
        else
            progCube_.UseProgram();
        
        glBindTextureUnit(5, envMap_.getHandle());
        glDepthMask(false);
        glBindVertexArray(dummyVAO_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(true);
    }
}
