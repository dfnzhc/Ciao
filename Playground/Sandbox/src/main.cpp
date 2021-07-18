#include <iostream>
#include <Ciao.h>

#include <glm.hpp>

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
        
    }

    void Shutdown() override
    {
        CIAO_INFO("Sandbox::Shutdown()");
        
    }

    void Update() override
    {
        //CIAO_INFO("Sandbox::Update()");
    }

    void Render() override
    {
        
        //CIAO_INFO("Sandbox::Render()");
        Ciao::Application::GetAppInst().GetRenderManager()->SetClearColour(glm::vec4{1.0, 0.0, 1.0, 1.0});
    }
};

Ciao::Scence* Ciao::CreateScence()
{
    return new Sandbox();
}

