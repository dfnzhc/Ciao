#pragma once

#include "Renderer/Shader.h"
#include "Texture.h"

namespace Ciao
{
    class Skybox
    {
    public:
        Skybox(
            const char* envMap = "Textures/immenstadter_horn_2k.hdr",
            const char* envMapIrradiance = "Textures/immenstadter_horn_2k_irradiance.hdr");
        ~Skybox();
        
        void Draw(std::shared_ptr<ShaderProgram> shader = nullptr);
    private:
        Texture envMap_;
        Texture envMapIrradiance_;
        Texture brdfLUT_ = {GL_TEXTURE_2D, GetAssetDir().append("Textures/brdfLUT.ktx").c_str()};
        Shader shdCubeVertex_ = Shader{ "Skybox.vert" };
        Shader shdCubeFragment_ = Shader{ "Skybox.frag" };
        ShaderProgram progCube_ = ShaderProgram{ shdCubeVertex_, shdCubeFragment_ };
        GLuint dummyVAO_;
    };
}


