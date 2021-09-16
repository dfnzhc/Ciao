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

const GLsizeiptr PerFrameBufferSize = sizeof(PerFrameData);

class VtxPulling : public Ciao::Scence
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "VtxPulling Scence";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("VtxPulling::Init()");

        LoadShaders();
        LoadTextures();

        m_pfb = make_shared<GLBuffer>(PerFrameBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_pfb->getHandle(), 0, PerFrameBufferSize);

        model_ = CreateRef<Model>();
        model_->Load(Asset_dir + "Models\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");


        grid = CreateRef<Grid>();
        grid->Create();

        glCreateVertexArrays(1, &skybox_vao);
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<GLBuffer> m_pfb;
    shared_ptr<Model> model_;

    shared_ptr<Grid> grid;

    GLuint skybox_vao;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_pfb->getHandle(), 0, PerFrameBufferSize, &perFrameData);

    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{1.0, 1.0, 1.0, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();
        
        m_Shaders[2]->UseProgram();
        
        modelMatrixStack.Push();
            modelMatrixStack.Translate(glm::vec3{0, 1, 0});
            // modelMatrixStack.Scale(glm::vec3{5.0});
            modelMatrixStack.RotateX(glm::radians(90.0f));
            m_Shaders[2]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[2]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            model_->Draw(m_Shaders[2]);
        modelMatrixStack.Pop();
        

        m_Shaders[1]->UseProgram();
        glBindVertexArray(skybox_vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        grid->Draw(m_Shaders[0]);
    }

    
    void Shutdown() override
    {
        CIAO_INFO("VtxPulling::Shutdown()");
        glDeleteVertexArrays(1, &skybox_vao);
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");
        ShaderFileNames.push_back("Skybox.vert");
        ShaderFileNames.push_back("Skybox.frag");
        ShaderFileNames.push_back("PBR\\PBR.vert");
        ShaderFileNames.push_back("PBR\\PBR.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- Grid 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- 天空盒 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {2, 3});
        
        /// 2 --- PBR 物体的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {4, 5}); 
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_albedo.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_AO.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_emissive.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_metalRoughness.jpg"});
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_normal.jpg"});
        
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\HDR\\Serpentine_Valley\\Serpentine_Valley_3k.hdr"});
        TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\HDR\\Serpentine_Valley\\Serpentine_Valley_Env.hdr"});
        TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brdfLUT.ktx"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        if (ImGui::Begin(u8"设置")) {
            ImGui::Text("Gogogo");
        }
        ImGui::End();
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new VtxPulling();
} 


extern inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
