#include <iostream>
#include <Ciao.h>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

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

class MeshDraw : public Ciao::Scence
{
public:
    Ciao::WindowProps GetWindowProps() override
    {
        Ciao::WindowProps props;
        props.Title = "MeshDraw Scence";

        return props;
    }

    void Init() override
    {
        CIAO_INFO("MeshDraw::Init()");

        LoadShaders();
        LoadTextures();

        m_pfb = make_shared<GLBuffer>(PerFrameBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_pfb->getHandle(), 0, PerFrameBufferSize);
        
        MeshData meshData;
        MeshFileHeader header = LoadMeshData((Asset_dir + "Models\\bistro\\Interior\\test.meshes").c_str(), meshData);

        mesh_ = CreateRef<GLMesh>(header, meshData.meshes_.data(), meshData.indexData_.data(), meshData.vertexData_.data());
        

        grid = CreateRef<Grid>();
        grid->Create();
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<GLBuffer> m_pfb;

    shared_ptr<GLMesh> mesh_;
    shared_ptr<Grid> grid;

    unsigned m_indicesSize;

    float m_Rot = 0.0f;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();

        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_pfb->getHandle(), 0, PerFrameBufferSize, &perFrameData);

        m_Rot += 0.5f;
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{1.0, 1.0, 1.0, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();
        
        m_Shaders[0]->UseProgram();
        
        modelMatrixStack.Push();
            modelMatrixStack.Scale(glm::vec3{2.0f});
            m_Shaders[0]->SetUniform("modelMatrix", modelMatrixStack.Top());
            m_Shaders[0]->SetUniform("normalMatrix", ComputeNormalMatrix(modelMatrixStack.Top()));
            mesh_->Draw(m_Shaders[0]);
        modelMatrixStack.Pop();
        
        grid->Draw(m_Shaders[1]);
    }

    
    void Shutdown() override
    {
        CIAO_INFO("MeshDraw::Shutdown()");
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("MeshDraw\\Mesh_Inst.vert");
        ShaderFileNames.push_back("MeshDraw\\Mesh_Inst.geom");
        ShaderFileNames.push_back("MeshDraw\\Mesh_Inst.frag");
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- Mesh 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1, 2});

        /// 1 --- Grid 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {3, 4});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\rubber_duck\\textures\\Duck_baseColor.png"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new MeshDraw();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
