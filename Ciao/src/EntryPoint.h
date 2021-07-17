#pragma once

#include "Application.h"
#include "Core/Renderer.h"
#include "Core/Log.h"

extern Ciao::Renderer* Ciao::CreateRenderer();

int main()
{
    Ciao::Log::Init();
    
    Ciao::Renderer* renderer = Ciao::CreateRenderer();
    Ciao::Application::GetAppInst().Execute(renderer);
    
    delete renderer;
    return 0;
}