#include <iostream>
#include <Ciao.h>

class Sandbox : public Ciao::Scence
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

Ciao::Scence* Ciao::CreateScence()
{
    return new Sandbox();
}
