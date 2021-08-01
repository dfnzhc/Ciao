#include <iostream>
#include <Ciao.h>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Ciao;
using namespace std;
using namespace glm;

const std::string Asset_dir{"..\\..\\Resources\\"};

struct PerFrameData
{
    mat4 view;
    mat4 proj;

    PerFrameData(const mat4& v, const mat4& p) :
        view(v), proj(p)
    {}
};

const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

class Sandbox : public Ciao::Scence
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "Sandbox Scence";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("Sandbox::Init()");

        uint32_t w, h;
        w = h = 0;
        Ciao::Application::GetInst().GetWindowSize(w, h);

        LoadShaders();
        LoadTextures();

        

        m_testBuffer = CreateRef<GLBuffer>(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_testBuffer->getHandle(), 0, kUniformBufferSize);
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;
    shared_ptr<Framebuffer> m_FBO;
    
    shared_ptr<GLBuffer> m_testBuffer;


public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix()};
        glNamedBufferSubData(m_testBuffer->getHandle(), 0, kUniformBufferSize, &perFrameData);
        
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.1, 0.1, 0.15, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

      
    }

    
    void Shutdown() override
    {
        CIAO_INFO("Sandbox::Shutdown()");
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        // ShaderFileNames.push_back("BaseShader.vert");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- 地板的 Shader 
        //AddShaderToPrograme(Shaders, m_Shaders, {0, 1});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        //TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brickwall.jpg"});


        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            glBindTextureUnit(i + 1, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        if (ImGui::Begin("Hello World")) {
            ImGui::Text("FPS %.1f FPS (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        }
        ImGui::End();
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new Sandbox();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
