#include <iostream>
#include <Ciao.h>

#include "imgui.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "stb_image.h"

using namespace Ciao;
using namespace std;
using namespace glm;

const std::string Asset_dir{"../../Resources/"};


struct PerFrameData
{
    mat4 view;
    mat4 proj;
    vec4 camPos;

    PerFrameData(const mat4& v, const mat4& p, const vec4& cp) :
        view(v), proj(p), camPos(cp)
    {}
};

struct SSAOParams
{
    float scale_ = 1.0f;
    float bias_ = 0.2f;
    float radius = 0.2f;
    float attScale = 1.0f;
    float distScale = 0.5f;
};

const GLsizeiptr PerFrameBufferSize = sizeof(PerFrameData);

class SceneTest : public Ciao::Demo
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "SceneTest Demo";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("SceneTest::Init()");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.2f, 0.3f, 0.7f, 1.0);

        uint32_t w, h;
        w = h = 0;
        Ciao::Application::GetInst().GetWindowSize(w, h);

        LoadShaders();
        LoadTextures();

        // SceneConvert(Asset_dir + "scene_San_Miguel.json");

        m_pfb = make_shared<GLBuffer>(PerFrameBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_pfb->getHandle(), 0, PerFrameBufferSize);

        grid = CreateRef<Grid>();
        grid->Create();

        sceneData1_ = new SceneData{ "Meshes/bistro_exterior.meshes", "Meshes/bistro_exterior.scene",
            "Meshes/bistro_exterior.materials" };
        scene1_ = CreateRef<GLScene>(*sceneData1_);

        sceneData2_ = new SceneData{ "Meshes/bistro_interior.meshes", "Meshes/bistro_interior.scene",
            "Meshes/bistro_interior.materials" };
        scene2_ = CreateRef<GLScene>(*sceneData2_);

        fbo_ = CreateRef<Framebuffer>(w, h);
        
        ssao_ = CreateRef<Framebuffer>(w, h, GL_RGBA8, 0);

        ssao_params_ = SSAOParams{};
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<Framebuffer> fbo_;
    shared_ptr<Framebuffer> ssao_;

    shared_ptr<GLBuffer> m_pfb;

    shared_ptr<Grid> grid;

    shared_ptr<GLScene> scene1_;
    SceneData* sceneData1_ = nullptr;

    shared_ptr<GLScene> scene2_;
    SceneData* sceneData2_ = nullptr;
    
    SSAOParams ssao_params_;

    bool bUseSSAO = false;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();
    }

    void Render() override
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_pfb->getHandle(), 0, PerFrameBufferSize, &perFrameData);
        

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();
        
        glClearNamedFramebufferfv(fbo_->getHandle(), GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
        glClearNamedFramebufferfi(fbo_->getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);
        fbo_->bind();
            scene1_->Draw(m_Shaders[1]);
            scene2_->Draw(m_Shaders[1]);
        fbo_->unbind();

        // 根据深度信息计算 ssao
        glClearNamedFramebufferfv(ssao_->getHandle(), GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
        glNamedBufferSubData(m_pfb->getHandle(), 0, sizeof(ssao_params_), &ssao_params_);
        ssao_->bind();
        m_Shaders[3]->UseProgram();
        glBindTextureUnit(0, fbo_->getTextureDepth().getHandle());
        glBindTextureUnit(1, m_Textures[0]->getHandle());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ssao_->unbind();

        // 模糊 ao 贴图，分成两次操作：一次水平，一次垂直
        if (bUseSSAO)
        {
            // x
            ssao_->bind();
            m_Shaders[4]->UseProgram();
            glBindTextureUnit(0, ssao_->getTextureColor().getHandle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
            ssao_->unbind();
            // y
            ssao_->bind();
            m_Shaders[5]->UseProgram();
            glBindTextureUnit(0, ssao_->getTextureColor().getHandle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
            ssao_->unbind();
        }
        
        // 合并输出
        if (bUseSSAO)
        {
            m_Shaders[6]->UseProgram();
            glBindTextureUnit(0, fbo_->getTextureColor().getHandle());
            glBindTextureUnit(1, ssao_->getTextureColor().getHandle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        else
        {
            m_Shaders[2]->UseProgram();
            glBindTextureUnit(0, fbo_->getTextureColor().getHandle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
    }

    
    void Shutdown() override
    {
        CIAO_INFO("SceneTest::Shutdown()");

        delete(sceneData1_);
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");
        ShaderFileNames.push_back("Scene/Scene.vert");
        ShaderFileNames.push_back("Scene/Scene.frag");
        ShaderFileNames.push_back("FullSceenQuad.vert");
        ShaderFileNames.push_back("OffSceen/test.frag");
        ShaderFileNames.push_back("SSAO/ssao.frag");
        ShaderFileNames.push_back("SSAO/BlurX.frag");
        ShaderFileNames.push_back("SSAO/BlurY.frag");
        ShaderFileNames.push_back("SSAO/CombineSSAO.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- Grid 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- 自定义 scene 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {2, 3});

        /// 2 --- 渲染 quad
        AddShaderToPrograme(Shaders, m_Shaders, {4, 5});

        /// 3 --- ssao
        AddShaderToPrograme(Shaders, m_Shaders, {4, 6});

        /// 4 --- gauss blur horizontal
        AddShaderToPrograme(Shaders, m_Shaders, {4, 7});

        /// 5 --- gauss blur vertical
        AddShaderToPrograme(Shaders, m_Shaders, {4, 8});

        /// 6 --- 合并最后的输出
        AddShaderToPrograme(Shaders, m_Shaders, {4, 9});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Textures/rot_texture.bmp"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            //glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        if (ImGui::Begin(u8"SSAO设置")) {
            ImGui::Checkbox(u8"使用SSAO", &bUseSSAO);
            ImGui::SliderFloat("scale", &ssao_params_.scale_, 0.0f, 2.0f);
            ImGui::SliderFloat("bias",  &ssao_params_.bias_, 0.0f, 0.3f);
            ImGui::SliderFloat("radius", &ssao_params_.radius, 0.05f, 0.5f);
            ImGui::SliderFloat("attenuation scale", &ssao_params_.attScale, 0.5f, 1.5f);
            ImGui::SliderFloat("distance scale", &ssao_params_.distScale, 0.0f, 1.0f);
        }
        ImGui::End();

        imguiTextureWindowGL("Frame Buffer", ssao_->getTextureColor().getHandle());
    }
};

Ciao::Demo* Ciao::CreateScence()
{
    return new SceneTest();
} 


extern inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
