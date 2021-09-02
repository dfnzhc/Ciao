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

class Shadow : public Ciao::Scence
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "Shadow Scence";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("Shadow::Init()");

        uint32_t w, h;
        w = h = 0;
        Ciao::Application::GetInst().GetWindowSize(w, h);

        LoadShaders();
        LoadTextures();

        plane = CreateRef<Plane>();
        plane->Init(10, 10, 5);

        sphere = CreateRef<Sphere>();
        sphere->Init(25, 25);
        
        m_LightPos = glm::vec3{-2.0f, 4.0f, 10.0f};

        mary = CreateRef<Model>();
        mary->Load(Asset_dir + "Models\\mary\\Marry.obj");

        m_FBO = CreateRef<Framebuffer>(2048, 2048);
        m_FBO->SetClearColour(glm::vec4{1.0});

        m_testBuffer = CreateRef<GLBuffer>(kUniformBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_testBuffer->getHandle(), 0, kUniformBufferSize);

        l_right = l_top = 7;
        l_left = l_bottom = -7;
        l_near = 0.1;
        l_far = 20;

        
        m_shadowFunc = 0;
        m_sampleCount = 50;
        m_filterSize = 10;
        m_bias = 0.001f;
        m_wlight = 1;
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;
    shared_ptr<Framebuffer> m_FBO;
    
    shared_ptr<Sphere> sphere;
    shared_ptr<Plane> plane;
    shared_ptr<Model> mary;
    
    shared_ptr<GLBuffer> m_testBuffer;

    glm::vec3 m_LightPos;

    float l_left, l_right, l_top, l_bottom, l_near, l_far;

    int m_shadowFunc;
    int m_sampleCount;
    int m_filterSize;
    float m_bias;
    int m_wlight;

    float m_Rot = 0.0f;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_testBuffer->getHandle(), 0, kUniformBufferSize, &perFrameData);
        
        glBindTextureUnit(7, m_FBO->getTextureDepth().getHandle());
        
        m_Rot += 0.5f;
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.0, 0.0, 0.0, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

        m_FBO->bind();
        
        m_Shaders[1]->UseProgram();
        mat4 lightProjection = ortho(l_left, l_right, l_bottom, l_top, l_near, l_far);
        mat4 lightView = lookAt(m_LightPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        m_Shaders[1]->SetUniform("lightSpaceMatrix", lightProjection * lightView);
        
        modelMatrixStack.Push();
            m_Shaders[1]->SetUniform("modelMatrix", modelMatrixStack.Top());
            plane->Draw(m_Shaders[1]);
        modelMatrixStack.Pop();
        
        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(-1.5, 0, 0));
            // modelMatrixStack.Scale(glm::vec3(20));
            m_Shaders[1]->SetUniform("modelMatrix", modelMatrixStack.Top());
            mary->Draw(m_Shaders[1]);
        modelMatrixStack.Pop();

        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(1.5, 0, 2));
            modelMatrixStack.Scale(glm::vec3(0.7));
            m_Shaders[1]->SetUniform("modelMatrix", modelMatrixStack.Top());
            mary->Draw(m_Shaders[1]);
        modelMatrixStack.Pop();
        
        m_FBO->unbind();

        m_Shaders[2]->UseProgram();
        m_Shaders[2]->SetUniform("lightPos", m_LightPos);
        m_Shaders[2]->SetUniform("withTexture", 0);
        m_Shaders[2]->SetUniform("lightSpaceMatrix", lightProjection * lightView);
        m_Shaders[2]->SetUniform("shadowFunc", m_shadowFunc);
        m_Shaders[2]->SetUniform("sampleCount", m_sampleCount);
        m_Shaders[2]->SetUniform("filterSize", m_filterSize);
        m_Shaders[2]->SetUniform("bias", m_bias);
        m_Shaders[2]->SetUniform("wLight", m_wlight);

        modelMatrixStack.Push();
            m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[2]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            plane->Draw(m_Shaders[2]);
        modelMatrixStack.Pop();

        
        m_Shaders[2]->SetUniform("withTexture", 1);
        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(-1.5, 0, 0));
            // modelMatrixStack.Translate(glm::vec3(0, -0.5, 0));
            // modelMatrixStack.Scale(glm::vec3(20));
            m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
            mary->Draw(m_Shaders[2]);
        modelMatrixStack.Pop();

        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(1.5, 0, 2));
            modelMatrixStack.Scale(glm::vec3(0.7));
            m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
            mary->Draw(m_Shaders[2]);
        modelMatrixStack.Pop();

        // modelMatrixStack.Push();
        //     modelMatrixStack.Translate(glm::vec3(0, -0.5, 0));
        //     modelMatrixStack.Scale(glm::vec3(20));
        //     m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
        //     m_Shaders[2]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
        //     mary->Draw(m_Shaders[2]);
        // modelMatrixStack.Pop();


        m_Shaders[0]->UseProgram();

        modelMatrixStack.Push();
            modelMatrixStack.Translate(m_LightPos);
            modelMatrixStack.Scale(glm::vec3(0.1));
            m_Shaders[0]->SetUniform("modelMatrix", modelMatrixStack.Top());
            sphere->Draw(m_Shaders[0]);
        modelMatrixStack.Pop();
    }

    
    void Shutdown() override
    {
        CIAO_INFO("Shadow::Shutdown()");
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("BaseShader.vert");
        ShaderFileNames.push_back("LightSource.frag");
        ShaderFileNames.push_back("shadow\\shadowMap.vert");
        ShaderFileNames.push_back("shadow\\shadowMap.frag");
        ShaderFileNames.push_back("Phong.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- 光源的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- shadow texture 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {2, 3});

        /// 2 --- Phong Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 4});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\mary\\MC003_Kozakura_Mari.png"});

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

        if (ImGui::Begin(u8"参数设置")) {
            ImGui::Text(u8"投影矩阵参数");
            ImGui::SliderFloat("Left", &l_left, -1, -10);
            ImGui::SliderFloat("Right", &l_right, 1, 10);
            ImGui::SliderFloat("Bottom", &l_bottom, -1, -10);
            ImGui::SliderFloat("Top", &l_top, 1, 10);
            ImGui::SliderFloat("Near", &l_near, -10, 10);
            ImGui::SliderFloat("Far", &l_far, 1, 100);
            ImGui::Text(u8"设置阴影方法");
            ImGui::RadioButton("UseShadow", &m_shadowFunc, 0);
            ImGui::RadioButton("PCF (Percentage Closer Filter)", &m_shadowFunc, 1);
            ImGui::RadioButton("PCSS (Percentage Closer Soft Shadow)", &m_shadowFunc, 2);
            ImGui::SliderInt(u8"采样数量", &m_sampleCount, 1, 100);
            ImGui::SliderInt(u8"采样半径", &m_filterSize, 1, 30);
            ImGui::SliderFloat(u8"深度偏移", &m_bias, 0.001, 0.1);
            
            ImGui::SliderInt(u8"PCSS - 光源宽度", &m_wlight, 1, 10);
            
            ImGui::SliderFloat3(u8"位置", (float*)&m_LightPos, -10.0, 10.0);
        }
        ImGui::End();

        
        if (ImGui::Begin(u8"中文测试")) {
            ImGui::Text(u8"白日依山尽");
            ImGui::Text(u8"黄河入海流");
        }
        ImGui::End();

        imguiTextureWindowGL("Shadow Map", m_FBO->getTextureDepth().getHandle());
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new Shadow();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
