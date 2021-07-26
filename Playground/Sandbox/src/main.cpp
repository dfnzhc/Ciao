#include <iostream>
#include <Ciao.h>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace Ciao;
using namespace std;

const std::string Asset_dir{"..\\..\\Resources\\"};

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

        // m_Skybox = CreateRef<Skybox>();
        // m_Skybox->Init(Asset_dir + "Textures\\skybox");
        // m_Skybox->SetTextureUnit(9);
        // 0 ++++  
        renderCommandVec.push_back(std::make_shared<DrawBuildInObj>(36, m_Shaders[0]));

        m_Mesh = CreateRef<OpenAssetImportMesh>();
        m_Mesh->Load(Asset_dir + "Models\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");

        m_ModelEntity = CreateRef<ModelEntity>();
        m_ModelEntity->Init(m_Mesh);
        m_ModelEntity->AddShader(m_Shaders[1]);
        m_ModelEntity->AddShader(m_Shaders[2]);

        // 1 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawModelEntity>(m_ModelEntity, 0));
        // 2 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawModelEntity>(m_ModelEntity, 1));

        m_Plane = CreateRef<Plane>();
        m_Plane->Init(30, 30, 10);
        // 3 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Plane, m_Shaders[3]));


        m_FBO = CreateRef<Framebuffer>(w, h);
        m_FBO->SetClearColour(glm::vec4{0.0});
    }

private:
    shared_ptr<Plane> m_Plane;
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;
    vector<shared_ptr<RenderCommand>> renderCommandVec;
    shared_ptr<OpenAssetImportMesh> m_Mesh;
    shared_ptr<ModelEntity> m_ModelEntity;
    shared_ptr<Framebuffer> m_FBO;

    float m_Rot = 0.0f;

public:
    void Update() override
    {
        // CIAO_INFO("Mouse Pos: {}, {}, Scroll: {}", Mouse::X(), Mouse::Y(), Mouse::GetScroll());
        m_Rot += 0.5f;
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.2, 0.3, 0.7, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();


        m_Shaders[1]->UseProgram();
        m_Shaders[1]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        m_Shaders[1]->SetUniform("cameraPos", Camera->GetPosition());
        m_Shaders[1]->SetUniform("viewMatrix", Camera->GetViewMatrix());
        
        modelMatrixStack.Push();
            // modelViewMatrixStack.RotateY(glm::radians(m_Rot));
            modelMatrixStack.RotateX(glm::radians(90.0f));
            modelMatrixStack.RotateZ(glm::radians(-120.0f));
            m_Shaders[1]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            m_Shaders[1]->SetUniform("modelMatrix",  modelMatrixStack.Top());
            CIAO_SUB_RC(renderCommandVec[1]);
        modelMatrixStack.Pop();


        m_Shaders[3]->UseProgram();
        m_Shaders[3]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        m_Shaders[3]->SetUniform("viewMatrix", Camera->GetViewMatrix());
        
        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3(0, -1.5, 0));
            m_Shaders[3]->SetUniform("modelMatrix", modelMatrixStack.Top());
            CIAO_SUB_RC(renderCommandVec[3]);
        modelMatrixStack.Pop();


        // CIAO_SUB_RC(std::make_shared<PushFramebuffer>(m_FBO));
        // m_Shaders[2]->UseProgram();
        // m_Shaders[2]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        // modelViewMatrixStack.Push();
        //     modelViewMatrixStack.RotateX(glm::radians(90.0f));
        //     modelViewMatrixStack.RotateZ(glm::radians(-120.0f));
        //     m_Shaders[2]->SetUniform("modelViewMatrix", modelViewMatrixStack.Top());
        //     CIAO_SUB_RC(renderCommandVec[2]);
        // modelViewMatrixStack.Pop();
        // CIAO_SUB_RC(std::make_shared<PopFramebuffer>());

        // 最后渲染天空盒
        m_Shaders[0]->UseProgram();
        m_Shaders[0]->SetUniform("projMatrix", Camera->GetProjectionMatrix());

        modelMatrixStack.Push();
            m_Shaders[0]->SetUniform("viewMatrix", glm::mat4(glm::mat3(Camera->GetViewMatrix())));
            CIAO_SUB_RC(renderCommandVec[0]);
        modelMatrixStack.Pop();
    }


    void Shutdown() override
    {
        CIAO_INFO("Sandbox::Shutdown()");
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("SkyboxShader.vert");
        ShaderFileNames.push_back("SkyboxShader.frag");
        ShaderFileNames.push_back("PBRShader.vert");
        ShaderFileNames.push_back("PBRShader.frag");
        ShaderFileNames.push_back("AOShader.vert");
        ShaderFileNames.push_back("AOShader.frag");
        ShaderFileNames.push_back("FloorShader.vert");
        ShaderFileNames.push_back("FloorShader.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序
        /// 0 --- 天空盒的 Shader
        auto skyShader = std::make_shared<ShaderProgram>();
        skyShader->CreateProgram();
        skyShader->AddShaderToProgram(&Shaders[0]);
        skyShader->AddShaderToProgram(&Shaders[1]);
        skyShader->LinkProgram();
        m_Shaders.push_back(skyShader);

        /// 1 --- 主要的 Shader
        auto mainShader = std::make_shared<ShaderProgram>();
        mainShader->CreateProgram();
        mainShader->AddShaderToProgram(&Shaders[2]);
        mainShader->AddShaderToProgram(&Shaders[3]);
        mainShader->LinkProgram();
        m_Shaders.push_back(mainShader);

        /// 2 --- AO Shader 
        auto aoShader = std::make_shared<ShaderProgram>();
        aoShader->CreateProgram();
        aoShader->AddShaderToProgram(&Shaders[4]);
        aoShader->AddShaderToProgram(&Shaders[5]);
        aoShader->LinkProgram();
        m_Shaders.push_back(aoShader);

        /// 3 --- 地板的 Shader 
        auto floorShader = std::make_shared<ShaderProgram>();
        floorShader->CreateProgram();
        floorShader->AddShaderToProgram(&Shaders[6]);
        floorShader->AddShaderToProgram(&Shaders[7]);
        floorShader->LinkProgram();
        m_Shaders.push_back(floorShader);
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_AO.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_emissive.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_albedo.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_metalRoughness.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_normal.jpg"});
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\piazza_bologni_1k.hdr"});
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\piazza_bologni_1k_irradiance.hdr"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brdfLUT.ktx"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\floor_tiles_06_diff_2k.jpg"});


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

            // ImVec2 size = { 480, 320 };
            // ImVec2 uv0 = { 0, 1 };
            // ImVec2 uv1 = { 1, 0 };
            // ImGui::Image((void*)(intptr_t) m_FBO->GetTextureId(), size, uv0, uv1);
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
