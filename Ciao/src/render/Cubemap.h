#pragma once
#include "GLShader.h"
#include "GLTexture.h"

namespace Ciao
{
    class Cubemap
    {
    public:
        Cubemap(const char* envMap = Res("Textures/immenstadter_horn_2k.hdr"));
        ~Cubemap();

        void Draw();
    
    private:
        GLTexture envMap_;
        
        GLShader shdCubemapVert_ = GLShader{ Res("Shaders/Skybox.vert") };
        GLShader shdCubemapFrag_ = GLShader{ Res("Shaders/Skybox.frag") };
        GLProgram progCubemap_ = GLProgram{ shdCubemapVert_, shdCubemapFrag_ };
        GLuint dummyVAO_;
    };
}
