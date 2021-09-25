#include <iostream>
#include <Ciao.h>

#include "imgui.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "imgui_internal.h"
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

struct HDRParams
{
    float exposure_ = 0.9f;
    float maxWhite_ = 1.17f;
    float bloomStrength_ = 1.1f;
	float adaptationSpeed_ = 0.1f;
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
        
        Ciao::Application::GetInst().GetWindowSize(width, height);

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

        fbo_ = CreateRef<Framebuffer>(width, height, GL_RGBA16F);
        brightPass_ = CreateRef<Framebuffer>(256, 256, GL_RGBA16F, 0);
        luminance_ = CreateRef<Framebuffer>(64, 64, GL_R16F, 0);
        bloom1_ = CreateRef<Framebuffer>(256, 256, GL_RGBA16F, 0);
        bloom2_ = CreateRef<Framebuffer>(256, 256, GL_RGBA16F, 0);

        glGenTextures(1, &luminance1x1);
        glTextureView(luminance1x1, GL_TEXTURE_2D, luminance_->getTextureColor().getHandle(), GL_R16F, 6, 1, 0, 1);
        const GLint Mask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
        glTextureParameteriv(luminance1x1, GL_TEXTURE_SWIZZLE_RGBA, Mask);

        glCreateVertexArrays(1, &skyVao_);

        hdr_params_ = HDRParams{};
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<Framebuffer> fbo_;
    shared_ptr<Framebuffer> brightPass_;
    shared_ptr<Framebuffer> luminance_;
    shared_ptr<Framebuffer> bloom1_;
    shared_ptr<Framebuffer> bloom2_;
    GLuint luminance1x1;

    shared_ptr<GLBuffer> m_pfb;

    shared_ptr<Grid> grid;

    shared_ptr<GLScene> scene1_;
    SceneData* sceneData1_ = nullptr;

    shared_ptr<GLScene> scene2_;
    SceneData* sceneData2_ = nullptr;

    GLuint skyVao_;

    bool bUseIBL_ = false;

    uint32_t width = 0, height = 0;

    HDRParams hdr_params_;
    bool bUseHDR = false;

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

        glEnable(GL_DEPTH_TEST);
        
        glClearNamedFramebufferfv(fbo_->getHandle(), GL_COLOR, 0, glm::value_ptr(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
        glClearNamedFramebufferfi(fbo_->getHandle(), GL_DEPTH_STENCIL, 0, 1.0f, 0);
        fbo_->bind();
            //渲染天空盒
            m_Shaders[3]->UseProgram();
            glDepthMask(false);
            glBindVertexArray(skyVao_);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(true);

            if(bUseIBL_)
            {
                scene1_->Draw(m_Shaders[1]);
                scene2_->Draw(m_Shaders[1]);
            }
            else
            {
                scene1_->Draw(m_Shaders[4]);
                scene2_->Draw(m_Shaders[4]);
            }
        fbo_->unbind();
        glGenerateTextureMipmap(fbo_->getTextureColor().getHandle());
        glTextureParameteri(fbo_->getTextureColor().getHandle(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        
        glDisable(GL_DEPTH_TEST);
        
        glNamedBufferSubData(m_pfb->getHandle(), 0, sizeof(hdr_params_), &hdr_params_);

        // 下采样，并把场景的颜色转换成明度（luminance）
        luminance_->bind();
        m_Shaders[6]->UseProgram();
        glBindTextureUnit(0, fbo_->getTextureColor().getHandle());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        luminance_->unbind();
        glGenerateTextureMipmap(luminance_->getTextureColor().getHandle());
        
        
        // 提取场景中的亮度 (只记录大于 1 的亮度，HDR）
        brightPass_->bind();
        m_Shaders[5]->UseProgram();
        glBindTextureUnit(0, fbo_->getTextureColor().getHandle());
        glDrawArrays(GL_TRIANGLES, 0, 6);
        brightPass_->unbind();
        
        glBlitNamedFramebuffer(brightPass_->getHandle(), bloom2_->getHandle(), 0, 0, 256, 256, 0, 0, 256,256, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        for (int i = 0; i != 4; i++)
        {
        	// 2.3 Blur X
        	bloom1_->bind();
        	m_Shaders[7]->UseProgram();
        	glBindTextureUnit(0, bloom2_->getTextureColor().getHandle());
        	glDrawArrays(GL_TRIANGLES, 0, 6);
        	bloom1_->unbind();
        	// 2.4 Blur Y
            bloom2_->bind();
            m_Shaders[8]->UseProgram();
            glBindTextureUnit(0, bloom1_->getTextureColor().getHandle());
            glDrawArrays(GL_TRIANGLES, 0, 6);
            bloom2_->unbind();
        }
        
        glViewport(0, 0, width, height);

        if (bUseHDR)
        {
            m_Shaders[9]->UseProgram();
            glBindTextureUnit(0, fbo_->getTextureColor().getHandle());
            glBindTextureUnit(1, luminance1x1);
            glBindTextureUnit(2, bloom2_->getTextureColor().getHandle());
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
        glDeleteVertexArrays(1, &skyVao_);
        glDeleteTextures(1, &luminance1x1);
        
        delete(sceneData1_);
        delete(sceneData2_);
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");
        ShaderFileNames.push_back("HDR/Scene.vert");
        ShaderFileNames.push_back("HDR/Scene.frag");
        ShaderFileNames.push_back("FullSceenQuad.vert");
        ShaderFileNames.push_back("OffSceen/test.frag");
        ShaderFileNames.push_back("Skybox.vert");
        ShaderFileNames.push_back("Skybox.frag");
        ShaderFileNames.push_back("Scene/Scene.frag");
        ShaderFileNames.push_back("HDR/BrightPass.frag");
        ShaderFileNames.push_back("HDR/ToLuminance.frag");
        ShaderFileNames.push_back("HDR/BlurX.frag");
        ShaderFileNames.push_back("HDR/BlurY.frag");
        ShaderFileNames.push_back("HDR/HDR.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- Grid 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- 自定义 scene 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {2, 3});

        /// 2 --- 渲染 quad
        AddShaderToPrograme(Shaders, m_Shaders, {4, 5});

        /// 3 --- 渲染天空盒
        AddShaderToPrograme(Shaders, m_Shaders, {6, 7});

        /// 4 --- 不使用IBL的着色器
        AddShaderToPrograme(Shaders, m_Shaders, {2, 8});

        /// 5 --- bright pass
        AddShaderToPrograme(Shaders, m_Shaders, {4, 9});

        /// 6 --- To luminance
        AddShaderToPrograme(Shaders, m_Shaders, {4, 10});

        /// 7 --- blur x
        AddShaderToPrograme(Shaders, m_Shaders, {4, 11});

        /// 8 --- blur y
        AddShaderToPrograme(Shaders, m_Shaders, {4, 12});

        /// 9 --- 合并 HDR
        AddShaderToPrograme(Shaders, m_Shaders, {4, 13});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures/immenstadter_horn_2k.HDR"});
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures/immenstadter_horn_2k_irradiance.HDR"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            //glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
        
        glBindTextureUnit(5, m_Textures[0]->getHandle());
        glBindTextureUnit(6, m_Textures[1]->getHandle());
    }

    void ImguiRender() override
    {
        if (ImGui::Begin(u8"HDR设置")) {
            ImGui::Checkbox(u8"使用IBL", &bUseIBL_);

            
            ImGui::Checkbox(u8"使用HDR", &bUseHDR);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !bUseHDR);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * bUseHDR ? 1.0f : 0.2f);

            ImGui::Separator();
            ImGui::Text(u8"平均亮度:");
            ImGui::Image((void*)(intptr_t)luminance1x1, ImVec2(128, 128), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
            ImGui::Separator();
            
            ImGui::SliderFloat("Exposure", &hdr_params_.exposure_, 0.1f, 2.0f);
            ImGui::SliderFloat("Max White", &hdr_params_.maxWhite_, 0.5f, 2.0f);
            ImGui::SliderFloat("Bloom strength", &hdr_params_.bloomStrength_, 0.0f, 2.0f);
		    ImGui::SliderFloat("Adaptation speed", &hdr_params_.adaptationSpeed_, 0.01f, 0.5f);
            ImGui::PopItemFlag();
            ImGui::PopStyleVar();
            
        }
        ImGui::End();

        imguiTextureWindowGL("Color", fbo_->getTextureColor().getHandle());
        imguiTextureWindowGL("Bright Pass", brightPass_->getTextureColor().getHandle());
        imguiTextureWindowGL("Luminance", luminance_->getTextureColor().getHandle());
        imguiTextureWindowGL("Bloom", bloom2_->getTextureColor().getHandle());
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
