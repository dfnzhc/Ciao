#include <iostream>
#include <Ciao.h>

#include "../../../Ciao/Deps/imgui/imgui.h"
#include "gtc/matrix_transform.hpp"

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
        
        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("SkyboxShader.vert");
        ShaderFileNames.push_back("SkyboxShader.frag");
        ShaderFileNames.push_back("MainShader.vert");
        ShaderFileNames.push_back("MainShader.frag");
        ShaderFileNames.push_back("AOShader.vert");
        ShaderFileNames.push_back("AOShader.frag");

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

        /// 2 --- 主要的 Shader 
        auto aoShader = std::make_shared<ShaderProgram>();
        aoShader->CreateProgram();
        aoShader->AddShaderToProgram(&Shaders[4]);
        aoShader->AddShaderToProgram(&Shaders[5]);
        aoShader->LinkProgram();
        m_Shaders.push_back(aoShader);
        

        m_Skybox = CreateRef<Skybox>();
        m_Skybox->Init(Asset_dir + "Textures\\skybox");
        // 0 ++++  
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Skybox, skyShader));

        m_Mesh = CreateRef<OpenAssetImportMesh>();
        m_Mesh->SetTexNames({"Diffuse", "Emissive", "Normal", "LightMap", "Unknown"});
        m_Mesh->Load(Asset_dir + "Models\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");
        m_Mesh->SetShader(mainShader);
        // 1 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Mesh, mainShader));
        // 2 ++++ 
        renderCommandVec.push_back(std::make_shared<DrawObject>(m_Mesh, aoShader));


        m_FBO = CreateRef<Framebuffer>(w, h);
    }

private:
    shared_ptr<Cube> m_refCube;
    vector<shared_ptr<ShaderProgram>> m_Shaders;
    vector<shared_ptr<RenderCommand>> renderCommandVec;
    shared_ptr<OpenAssetImportMesh> m_Mesh;
    shared_ptr<Skybox> m_Skybox;
    shared_ptr<Framebuffer> m_FBO;

public:
    void Update() override
    {
        // CIAO_INFO("Mouse Pos: {}, {}, Scroll: {}", Mouse::X(), Mouse::Y(), Mouse::GetScroll());
    }

    void Render() override
    {
        Ciao::Application::GetInst().GetRenderManager()->SetClearColour(glm::vec4{0.2, 0.3, 0.7, 1.0});
        auto Camera = Ciao::Application::GetInst().GetCamera();

        glutil::MatrixStack modelViewMatrixStack;
        modelViewMatrixStack.SetMatrix(Camera->GetViewMatrix());

        m_Shaders[1]->UseProgram();
        m_Shaders[1]->SetUniform("projMatrix", Camera->GetProjectionMatrix());

        Ciao::Application::GetInst().GetRenderManager()->Submit(std::make_shared<PushFramebuffer>(m_FBO));
        modelViewMatrixStack.Push();
            modelViewMatrixStack.Translate(glm::vec3(-2.0f, 0.0f, 0.0f));
            modelViewMatrixStack.RotateX(glm::radians(90.0f));
            modelViewMatrixStack.RotateZ(glm::radians(-120.0f));
            m_Shaders[1]->SetUniform("modelViewMatrix", modelViewMatrixStack.Top());
            Ciao::Application::GetInst().GetRenderManager()->Submit(renderCommandVec[1]);
        modelViewMatrixStack.Pop();
        Ciao::Application::GetInst().GetRenderManager()->Submit(std::make_shared<PopFramebuffer>());
        
        // modelViewMatrixStack.Push();
        //     modelViewMatrixStack.Translate(glm::vec3(2.0f, 0.0f, 0.0f));
        //     modelViewMatrixStack.RotateX(glm::radians(90.0f));
        //     modelViewMatrixStack.RotateZ(glm::radians(-120.0f));
        //     m_Shaders[2]->SetUniform("modelViewMatrix", modelViewMatrixStack.Top());
        //     
        //     Ciao::Application::GetInst().GetRenderManager()->Submit(renderCommandVec[2]);
        //     
        // modelViewMatrixStack.Pop();
        
        
        // 最后渲染天空盒
        m_Shaders[0]->UseProgram();
        m_Shaders[0]->SetUniform("skybox", 0);
        m_Shaders[0]->SetUniform("projMatrix", Camera->GetProjectionMatrix());
        
        modelViewMatrixStack.Push();
            m_Shaders[0]->SetUniform("viewMatrix", glm::mat4(glm::mat3(Camera->GetViewMatrix())));
            Ciao::Application::GetInst().GetRenderManager()->Submit(renderCommandVec[0]);
        modelViewMatrixStack.Pop();
    }

    
    void Shutdown() override
    {
        CIAO_INFO("Sandbox::Shutdown()");
        
    }

    void ImguiRender() override
    {
        if (ImGui::Begin("Hello World")) {
            ImGui::Text("FPS %.1f FPS (%.3f ms/f)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            
            ImVec2 size = { 480, 320 };
            ImVec2 uv0 = { 0, 1 };
            ImVec2 uv1 = { 1, 0 };
            ImGui::Image((void*)(intptr_t) m_FBO->GetTextureId(), size, uv0, uv1);
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
