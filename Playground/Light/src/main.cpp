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
    vec4 camPos;

    PerFrameData(const mat4& v, const mat4& p, const vec4& cp) :
        view(v), proj(p), camPos(cp)
    {}
};

const GLsizeiptr kUniformBufferSize = sizeof(PerFrameData);

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

        plane = CreateRef<Plane>();
        plane->Init(10, 10, 5);

        sphere = CreateRef<Sphere>();
        sphere->Init(25, 25);

        m_Light = CreateRef<GLLight>();
        m_Light->Position = glm::vec4{-2.0f, 4.0f, -1.0f, 1.0};
        m_Light->Direction = glm::vec4{-1.0};
        m_Light->ambient = glm::vec4{0.2f};
        m_Light->diffuse = glm::vec4{0.8f};
        m_Light->specular = glm::vec4{1};
        m_Light->cutOff = 60.0f;
        m_Light->theta = 45.0f;

        bunny = CreateRef<Model>();
        bunny->Load(Asset_dir + "Models\\bunny.obj");

        m_FBO = CreateRef<Framebuffer>(2048, 2048);
        m_FBO->SetClearColour(glm::vec4{1.0});

        m_testBuffer = CreateRef<GLBuffer>(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_testBuffer->getHandle(), 0, kUniformBufferSize);

        m_lightBuffer = CreateRef<GLBuffer>(LightBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_lightBuffer->getHandle(), 0, LightBufferSize);

        wLight = 13;
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;
    shared_ptr<Framebuffer> m_FBO;
    
    shared_ptr<Sphere> sphere;
    shared_ptr<Plane> plane;
    shared_ptr<Model> bunny;
    
    shared_ptr<GLBuffer> m_testBuffer;
    shared_ptr<GLBuffer> m_lightBuffer;

    shared_ptr<GLLight> m_Light;
    int LightType;
    int wLight;

    float m_Rot = 0.0f;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_testBuffer->getHandle(), 0, kUniformBufferSize, &perFrameData);

        const GLLight lightData{*m_Light.get()};
        glNamedBufferSubData(m_lightBuffer->getHandle(), 0, LightBufferSize, &lightData);
        
        glBindTextureUnit(7, m_FBO->getTextureDepth().getHandle());
        
        m_Rot += 0.5f;
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.1, 0.1, 0.15, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

        m_FBO->bind();
        
        m_Shaders[3]->UseProgram();
        mat4 lightProjection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 20.5f);
        mat4 lightView = lookAt(vec3(m_Light->Position), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        m_Shaders[3]->SetUniform("lightSpaceMatrix", lightProjection * lightView);
        
        modelMatrixStack.Push();
            //modelMatrixStack.Translate(glm::vec3(0, -1.5, 0));
            m_Shaders[3]->SetUniform("modelMatrix", modelMatrixStack.Top());
            plane->Draw(m_Shaders[3]);
        modelMatrixStack.Pop();
        
        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(0, -0.5, 0));
            modelMatrixStack.Scale(glm::vec3(20));
            m_Shaders[3]->SetUniform("modelMatrix", modelMatrixStack.Top());
            bunny->Draw(m_Shaders[3]);
        modelMatrixStack.Pop();
        
        m_FBO->unbind();

        m_Shaders[0]->UseProgram();
        //m_Shaders[0]->SetUniform("camPos", Camera->GetPosition());
        m_Shaders[0]->SetUniform("LightType", LightType);
        m_Shaders[0]->SetUniform("lightSpaceMatrix", lightProjection * lightView);
        m_Shaders[0]->SetUniform("wLight", wLight);

        modelMatrixStack.Push();
            //modelMatrixStack.Translate(glm::vec3(0, -1.5, 0));
            m_Shaders[0]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[0]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            plane->Draw(m_Shaders[0]);
        modelMatrixStack.Pop();

        m_Shaders[2]->UseProgram();
        //m_Shaders[2]->SetUniform("camPos", Camera->GetPosition());
        m_Shaders[2]->SetUniform("LightType", LightType);

        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(0, -0.5, 0));
            modelMatrixStack.Scale(glm::vec3(20));
            m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[2]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            bunny->Draw(m_Shaders[2]);
        modelMatrixStack.Pop();


        m_Shaders[1]->UseProgram();

        modelMatrixStack.Push();
            modelMatrixStack.Translate(m_Light->Position);
            modelMatrixStack.Scale(glm::vec3(0.3));
            m_Shaders[1]->SetUniform("modelMatrix", modelMatrixStack.Top());
            sphere->Draw(m_Shaders[1]);
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
        ShaderFileNames.push_back("BaseShader.vert");
        ShaderFileNames.push_back("SingleTexture_PCSS.frag");
        ShaderFileNames.push_back("LightSource.frag");
        ShaderFileNames.push_back("BaseColor.frag");
        ShaderFileNames.push_back("shadowMapShader.vert");
        ShaderFileNames.push_back("shadowMapShader.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- 地板的 Shader 
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- 光源的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 2});

        /// 2 --- bunny 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 3});

        /// 3 --- shadow texture 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {4, 5});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brickwall.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brickwall_normal.jpg"});


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
            auto Camera = Ciao::Application::GetInst().GetCamera();
            ImGui::Text("Camera Pos %.1f, %.1f, %.1f", Camera->GetPosition().x, Camera->GetPosition().y,
                        Camera->GetPosition().z);
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
            ImGui::SliderInt("Light Width", (int*)&wLight, 1, 50);

        }
        ImGui::End();


        imguiTextureWindowGL("Depth", m_FBO->getTextureDepth().getHandle());
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
