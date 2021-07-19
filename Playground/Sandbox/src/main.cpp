#include <iostream>
#include <Ciao.h>

#include "gtc/matrix_transform.hpp"

using namespace Ciao;

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

        std::vector<Shader> Shaders;
        std::vector<std::string> ShaderFileNames;
        ShaderFileNames.push_back("shader.vert");
        ShaderFileNames.push_back("shader.frag");

        ReadShaderFile(ShaderFileNames, Shaders);

        // 创建 OpenGL Shader 程序
        m_refShader = std::make_shared<ShaderProgram>();
        m_refShader->CreateProgram();
        m_refShader->AddShaderToProgram(&Shaders[0]);
        m_refShader->AddShaderToProgram(&Shaders[1]);
        m_refShader->LinkProgram();

        auto m_refCube = CreateRef<Cube>();
        m_refCube->Init(Asset_Dir + "Textures\\container.jpg");
        
        auto oc = std::make_shared<ObjRenderCommand>(m_refCube, m_refShader);
        
        renderCommandVec.push_back(oc);
    }

    std::shared_ptr<Cube> m_refCube;
    std::shared_ptr<ShaderProgram> m_refShader;
    std::vector<std::shared_ptr<RenderCommand>> renderCommandVec;

    void Shutdown() override
    {
        CIAO_INFO("Sandbox::Shutdown()");
        
    }

    void Update() override
    {
        // CIAO_INFO("Mouse Pos: {}, {}, Scroll: {}", Mouse::X(), Mouse::Y(), Mouse::GetScroll());
    }

    void Render() override
    {
        Ciao::Application::GetAppInst().GetRenderManager()->SetClearColour(glm::vec4{0.3, 0.6, 0.8, 1.0});
        auto Camera = Ciao::Application::GetAppInst().GetCamera();
        
        m_refShader->UseProgram();
        m_refShader->SetUniform("tex1", 0);
        
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        glm::mat4 view          = Camera->GetViewMatrix();
        glm::mat4 projection    = Camera->GetProjectionMatrix();
        
        m_refShader->SetUniform("modelViewMatrix", view * model);
        m_refShader->SetUniform("projMatrix", projection);
        
        Ciao::Application::GetAppInst().GetRenderManager()->Submit(renderCommandVec[0]);
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new Sandbox();
}

