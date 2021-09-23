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
    mat4 mvp;
};

const GLsizeiptr PerFrameBufferSize = sizeof(PerFrameData);

class SceneTest : public Ciao::Demo
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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.2f, 0.3f, 0.7f, 1.0);

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
            vertices.push_back({.pos = vec3(v.x, v.z, v.y), .tc = vec2(t.x, t.y)});
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
        // glVertexArrayVertexBuffer(m_vao, 0, m_dataVertices, 0, sizeof(vec3) + sizeof(vec2));
        //
        // glEnableVertexArrayAttrib(m_vao, 0);
        // glVertexArrayAttribFormat(m_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        // glVertexArrayAttribBinding(m_vao, 0, 0);
        //
        // glEnableVertexArrayAttrib(m_vao, 1);
        // glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(vec3));
        // glVertexArrayAttribBinding(m_vao, 1, 0);
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_dataVertices);

        glBindVertexArray(0);
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<GLBuffer> m_pfb;

    GLuint m_dataIndices;
    GLuint m_dataVertices;
    GLuint m_vao;

    GLuint m_tex;

    unsigned m_indicesSize;

public:
    void Update() override
    {
        auto Camera = Ciao::Application::GetInst().GetCamera();

        const mat4 m = scale(mat4(1.0), vec3{3.0});

        const PerFrameData perFrameData = {.mvp = Camera->GetProjectionMatrix() * Camera->GetViewMatrix() * m};
        glNamedBufferSubData(m_pfb->getHandle(), 0, PerFrameBufferSize, &perFrameData);
    }

    void Render() override
    {
        glClearColor(0.2f, 0.3f, 0.7f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

        m_Shaders[0]->UseProgram();
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indicesSize, GL_UNSIGNED_INT, nullptr);
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

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- vtxpulling 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1, 2});
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        TexInfo.push_back({GL_TEXTURE_2D, "Models\\rubber_duck\\textures\\Duck_baseColor.png"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i)
        {
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

Ciao::Demo* Ciao::CreateScence()
{
    return new SceneTest();
}


inline std::string Ciao::GetAssetDir()
{
    return Asset_dir;
}
