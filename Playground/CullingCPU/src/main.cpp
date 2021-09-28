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

        LoadShaders();
        // LoadTextures();

        // SceneConvert(Asset_dir + "scene.json");
        //MergeBistro();

        m_pfb = make_shared<GLBuffer>(PerFrameBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_pfb->getHandle(), 0, PerFrameBufferSize);

        grid = CreateRef<Grid>();
        grid->Create();

        sceneData_ = new SceneData{ "Meshes/bistro_all.meshes", "Meshes/bistro_all.scene",
            "Meshes/bistro_all.materials" };
        scene_ = CreateRef<GLMesh<SceneData>>(*sceneData_);

        skybox_ = CreateRef<Skybox>();

        canvas_ = CreateRef<CanvasGL>();

        for (const auto& c : sceneData_->shapes_)
        {
            const mat4 model = sceneData_->scene_.globalTransform_[c.transformIndex];
            sceneData_->meshData_.boxes_[c.meshIndex].transform(model);
        }

        fullScene = CreateRef<BoundingBox>();
        *fullScene = combineBoxes(sceneData_->meshData_.boxes_);
    }

private:
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<Texture>> m_Textures;

    shared_ptr<GLBuffer> m_pfb;

    shared_ptr<Grid> grid;

    shared_ptr<GLMesh<SceneData>> scene_;
    SceneData* sceneData_ = nullptr;

    shared_ptr<Skybox> skybox_;

    shared_ptr<BoundingBox> fullScene;

    shared_ptr<CanvasGL> canvas_;
    
    mat4 CullingView = Ciao::Application::GetInst().GetCamera()->GetViewMatrix();
    bool bFreezeCullingView = false;
    bool bDrawMeshes = true;
    bool bDrawBoxes = true;
    bool bDrawGrid = true;

    int numVisibleMeshes = 0;

public:
    void Update() override
    {
        
    }

    void Render() override
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glutil::MatrixStack modelMatrixStack;
        modelMatrixStack.SetIdentity();

        auto Camera = Ciao::Application::GetInst().GetCamera();
        
        const PerFrameData perFrameData{Camera->GetViewMatrix(), Camera->GetProjectionMatrix(), vec4(Camera->GetPosition(), 1.0)};
        glNamedBufferSubData(m_pfb->getHandle(), 0, PerFrameBufferSize, &perFrameData);

        if (bFreezeCullingView)
            CullingView = Camera->GetViewMatrix();

        vec4 frustumPlanes[6];
        getFrustumPlanes(Camera->GetProjectionMatrix() * CullingView, frustumPlanes);
        vec4 frustumCorners[8];
        getFrustumCorners(Camera->GetProjectionMatrix() * CullingView, frustumCorners);

        
        {
            DrawElementsIndirectCommand* cmd = scene_->bufferIndirect_.drawCommands_.data();
            for (const auto& c : sceneData_->shapes_)
            {
                cmd->instanceCount = isBoxInFrustum(frustumPlanes, frustumCorners, sceneData_->meshData_.boxes_[c.meshIndex]) ? 1 : 0;
                numVisibleMeshes += (cmd++)->instanceCount;
            }
            scene_->bufferIndirect_.uploadIndirectBuffer();
        }

        if (bDrawBoxes)
        {
            DrawElementsIndirectCommand* cmd = scene_->bufferIndirect_.drawCommands_.data();
            for (const auto& c : sceneData_->shapes_)
                drawBox3dGL(*canvas_, mat4(1.0f), sceneData_->meshData_.boxes_[c.meshIndex], (cmd++)->instanceCount ? vec4(0, 1, 0, 1) : vec4(1, 0, 0, 1));
            drawBox3dGL(*canvas_, mat4(1.0f), *fullScene, vec4(1, 0, 0, 1));
        }
        
        skybox_->Draw();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        if(bDrawMeshes)
        {
            m_Shaders[1]->UseProgram();
            scene_->draw(sceneData_->shapes_.size());
        }

        if (bDrawGrid)
            grid->Draw(m_Shaders[0]);

        if(bFreezeCullingView)
            renderCameraFrustumGL(*canvas_, CullingView,
                Ciao::Application::GetInst().GetCamera()->GetProjectionMatrix(), vec4(1, 1, 0, 1), 100);
        canvas_->flush();
    }

    
    void Shutdown() override
    {
        CIAO_INFO("SceneTest::Shutdown()");

        delete(sceneData_);
    }

    void LoadShaders()
    {
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("Grid.vert");
        ShaderFileNames.push_back("Grid.frag");
        ShaderFileNames.push_back("Scene/Scene.vert");
        ShaderFileNames.push_back("Scene/Scene.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序

        /// 0 --- Grid 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {0, 1});

        /// 1 --- 自定义 scene 的 Shader
        AddShaderToPrograme(Shaders, m_Shaders, {2, 3});
        
    }

    void LoadTextures()
    {
        // 以 Textures 目录作为根目录
        std::vector<std::pair<GLenum, string>> TexInfo;
        // TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_albedo.jpg"});
        // TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_AO.jpg"});
        // TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_emissive.jpg"});
        // TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_metalRoughness.jpg"});
        // TexInfo.push_back({GL_TEXTURE_2D, "Models\\DamagedHelmet\\glTF\\Default_normal.jpg"});
        //
        // TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\HDR\\Serpentine_Valley\\Serpentine_Valley_3k.hdr"});
        // TexInfo.push_back({GL_TEXTURE_CUBE_MAP, "Textures\\HDR\\Serpentine_Valley\\Serpentine_Valley_Env.hdr"});
        // TexInfo.push_back({GL_TEXTURE_2D, "Textures\\brdfLUT.ktx"});

        for (unsigned int i = 0; i < TexInfo.size(); ++i) {
            auto Tex = CreateRef<Texture>(
                TexInfo[i].first, std::string(Asset_dir + TexInfo[i].second).c_str());
            glBindTextureUnit(i, Tex->getHandle());

            m_Textures.push_back(Tex);
        }
    }

    void ImguiRender() override
    {
        if (ImGui::Begin("Control panel")) {
            ImGui::Text("Draw:");
            ImGui::Checkbox("Meshes", &bDrawMeshes);
            ImGui::Checkbox("Boxes",  &bDrawBoxes);
            ImGui::Checkbox("Grid",  &bDrawGrid);
            ImGui::Separator();
            ImGui::Checkbox("Freeze culling frustum (P)", &bFreezeCullingView);
            ImGui::Separator();
            ImGui::Text("Visible meshes: %i", numVisibleMeshes);
        }
        ImGui::End();
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
