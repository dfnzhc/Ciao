#include <iostream>
#include <Ciao.h>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Ciao;
using namespace std;

const std::string Asset_dir{"..\\..\\Resources\\"};

class Light : public Ciao::Scence
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "Lighting Scence";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("Light::Init()");

        uint32_t w, h;
        w = h = 0;
        Ciao::Application::GetInst().GetWindowSize(w, h);

        LoadShaders();
        LoadTextures();

        m_Plane = CreateRef<Plane>();
        m_Plane->Init(20, 20, 5);
        // 1 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Plane, m_Shaders[0]));

        m_Sphere = CreateRef<Sphere>();
        m_Sphere->Init(25, 25);
        // 2 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Sphere, m_Shaders[1]));

        m_Light = CreateRef<GLLight>();
        m_Light->Position = glm::vec3{0.0, 1.0, 0.0};
        m_Light->Direction = glm::vec3{0.0, -1.0, 0.0};
        m_Light->ambient = glm::vec3{0.2f};
        m_Light->diffuse = glm::vec3{0.8f};
        m_Light->specular = glm::vec3{1};
        m_Light->cutOff = 60.0f;
        m_Light->theta = 45.0f;

        // m_FBO = CreateRef<Framebuffer>(w, h);
        // m_FBO->SetClearColour(glm::vec4{0.0});
    }

private:
    shared_ptr<Sphere> m_Sphere;
    shared_ptr<Plane> m_Plane;
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;
    vector<shared_ptr<RenderCommand>> renderCommandVec;
    shared_ptr<Framebuffer> m_FBO;

    shared_ptr<GLLight> m_Light;
    int LightType;

    float m_Rot = 0.0f;

public:
    void Update() override
    {
        // CIAO_INFO("Mouse Pos: {}, {}, Scroll: {}", Mouse::X(), Mouse::Y(), Mouse::GetScroll());
        m_Rot += 0.5f;
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.1, 0.1, 0.15, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

        m_Shaders[0]->UseProgram();
        m_Shaders[0]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        m_Shaders[0]->SetUniform("viewMatrix", Camera->GetViewMatrix());
        m_Shaders[0]->SetUniform("camPos", Camera->GetPosition());
        m_Shaders[0]->SetUniform("light.pos", m_Light->Position);
        m_Shaders[0]->SetUniform("light.dir", m_Light->Direction);
        m_Shaders[0]->SetUniform("light.ambient", m_Light->ambient);
        m_Shaders[0]->SetUniform("light.diffuse", m_Light->diffuse);
        m_Shaders[0]->SetUniform("light.specular", m_Light->specular);
        m_Shaders[0]->SetUniform("light.cutoff", cos(glm::radians(m_Light->cutOff)));
        m_Shaders[0]->SetUniform("light.theta", cos(glm::radians(m_Light->theta)));
        m_Shaders[0]->SetUniform("LightType", LightType);

        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(0, -1.5, 0));
            m_Shaders[0]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[0]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            CIAO_SUB_RC(renderCommandVec[0]);
        modelMatrixStack.Pop();


        m_Shaders[1]->UseProgram();
        m_Shaders[1]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        m_Shaders[1]->SetUniform("viewMatrix", Camera->GetViewMatrix());

        modelMatrixStack.Push();
            modelMatrixStack.Translate(m_Light->Position);
            modelMatrixStack.Scale(glm::vec3(0.3));
            m_Shaders[1]->SetUniform("modelMatrix", modelMatrixStack.Top());
            CIAO_SUB_RC(renderCommandVec[1]);
        modelMatrixStack.Pop();
    }


    void Shutdown() override
    {
        CIAO_INFO("Light::Shutdown()");
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("FloorShader.vert");
        ShaderFileNames.push_back("FloorShader.frag");
        ShaderFileNames.push_back("LightShader.vert");
        ShaderFileNames.push_back("LightShader.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- 地板的 Shader 
        auto floorShader = std::make_shared<ShaderProgram>();
        floorShader->CreateProgram();
        floorShader->AddShaderToProgram(&Shaders[0]);
        floorShader->AddShaderToProgram(&Shaders[1]);
        floorShader->LinkProgram();
        m_Shaders.push_back(floorShader);

        /// 1 --- 光源的 Shader 
        auto lightShader = std::make_shared<ShaderProgram>();
        lightShader->CreateProgram();
        lightShader->AddShaderToProgram(&Shaders[2]);
        lightShader->AddShaderToProgram(&Shaders[3]);
        lightShader->LinkProgram();
        m_Shaders.push_back(lightShader);
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\FloorTiles\\broken_wall_diff_2k.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\FloorTiles\\broken_wall_disp_2k.png"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\FloorTiles\\broken_wall_rough_2k.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\FloorTiles\\broken_wall_nor_gl_2k.hdr"});


        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        if (ImGui::Begin("Hello World")) {
            ImGui::Text("FPS %.1f FPS (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            auto Camera = Ciao::Application::GetInst().GetCamera();
            ImGui::Text("Camera Pos %.1f, %.1f, %.1f", Camera->GetPosition().x, Camera->GetPosition().y, Camera->GetPosition().z);

            // ImVec2 size = { 480, 320 };
            // ImVec2 uv0 = { 0, 1 };
            // ImVec2 uv1 = { 1, 0 };
            // ImGui::Image((void*)(intptr_t) m_FBO->GetTextureId(), size, uv0, uv1);
        }
        ImGui::End();

        if (ImGui::Begin("Light props")) {
            ImGui::Combo("Light Source Type", &LightType, "Directional Light\0Point Light\0Spot Light\0");
            ImGui::DragFloat3("Position", (float*)&m_Light->Position);
            ImGui::DragFloat3("Direction", (float*)&m_Light->Direction);

            ImGui::ColorEdit3("Ambient", (float*)&m_Light->ambient);
            ImGui::ColorEdit3("Diffuse", (float*)&m_Light->diffuse);
            ImGui::ColorEdit3("Specular", (float*)&m_Light->specular);
            
            ImGui::SliderFloat("Cutoff", (float*)&m_Light->cutOff, 50, 80);
            ImGui::SliderFloat("Theta", (float*)&m_Light->theta, 30, 50);

        }
        ImGui::End();
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new Light();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
