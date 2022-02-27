#include "pch.h"
#include "Cubemap.h"

namespace Ciao
{
    Cubemap::Cubemap(const char* envMapFile)
        :envMap_(GL_TEXTURE_CUBE_MAP, envMapFile)
    {
        glCreateVertexArrays(1, &dummyVAO_);
    }

    Cubemap::~Cubemap()
    {
        glDeleteVertexArrays(1, &dummyVAO_);
    }

    void Cubemap::Draw()
    {
        progCubemap_.useProgram();
        
        const GLuint textures[] = {envMap_.getHandle()};
        glBindTextures(7, 1, textures);
        //glBindTextureUnit(5, envMap_.getHandle());

        glDepthFunc(GL_LEQUAL);
        //glDepthMask(false);
        glBindVertexArray(dummyVAO_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        //glDepthMask(true);
    }
}
