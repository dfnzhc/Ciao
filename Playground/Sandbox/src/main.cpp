#include <iostream>
#include <Ciao.h>

class Sandbox : public Ciao::Renderer
{
public:
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
    }
};

Ciao::Renderer* Ciao::CreateRenderer()
{
    return new Sandbox();
}
