#include <iostream>
#include <Ciao.h>

#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "stb_image.h"
#include "../../../Ciao/Deps/sdl2/include/SDL_timer.h"

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
        
        // load model
        const aiScene* scene = aiImportFile((Asset_dir + "Models\\rubber_duck\\scene.gltf").c_str(), aiProcess_Triangulate);

        if (!scene || !scene->HasMeshes())
            CIAO_ASSERT("Unable to load data.");

        struct VertexData
        {
            vec3 pos;
            vec2 tc;
        };

        const aiMesh* mesh = scene->mMeshes[0];
        std::vector<VertexData> vertices;
        for (unsigned i = 0; i != mesh->mNumVertices; i++)
        {
            const aiVector3D v = mesh->mVertices[i];
            const aiVector3D t = mesh->mTextureCoords[0][i];
            vertices.push_back({ .pos = vec3(v.x, v.z, v.y), .tc = vec2(t.x, t.y) });
        }

        std::vector<unsigned int> indices;
        for (unsigned i = 0; i != mesh->mNumFaces; i++)
        {
            for (unsigned j = 0; j != 3; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
        aiReleaseImport(scene);

        const size_t kSizeIndices = sizeof(unsigned int) * indices.size();
        const size_t kSizeVertices = sizeof(VertexData) * vertices.size();

        // indices
        glCreateBuffers(1, &m_dataIndices);
        glNamedBufferStorage(m_dataIndices, kSizeIndices, indices.data(), 0);
        m_indicesSize = static_cast<unsigned>(kSizeIndices);

        glCreateVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glVertexArrayElementBuffer(m_vao, m_dataIndices);

        // vertices
        glCreateBuffers(1, &m_dataVertices);
        glNamedBufferStorage(m_dataVertices, kSizeVertices, vertices.data(), 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_dataVertices);

        grid = CreateRef<Grid>();
        grid->Create();
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<GLBuffer> m_pfb;

    GLuint m_dataIndices;
    GLuint m_dataVertices;
    GLuint m_vao;

    GLuint m_tex;

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
        glBindVertexArray(m_vao);
        
        modelMatrixStack.Push();
            modelMatrixStack.Scale(glm::vec3{3.0});
            modelMatrixStack.RotateY(m_Rot * 0.001f);
            m_Shaders[0]->SetUniform("modelMatrix", modelMatrixStack.Top());
            glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, nullptr);
        modelMatrixStack.Pop();
        
        grid->Draw(m_Shaders[1]);
    }

    
    void Shutdown() override
    {
        CIAO_INFO("VtxPulling::Shutdown()");
        glDeleteBuffers(1, &m_dataIndices);
        glDeleteBuffers(1, &m_dataVertices);
        glDeleteVertexArrays(1, &m_vao);
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("vtxpulling\\vp.vert");
        ShaderFileNames.push_back("vtxpulling\\vp.geom");
        ShaderFileNames.push_back("vtxpulling\\vp.frag");
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- vtxpulling 的 Shader
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
    return new VtxPulling();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
