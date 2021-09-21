#pragma once

#include "Application.h"
#include "Core/Demo.h"
#include "Core/Log.h"

extern Ciao::Demo* Ciao::CreateScence();

int main()
{
    Ciao::Log::Init();
    
    Ciao::Demo* scence = Ciao::CreateScence();
    Ciao::Application::GetInst().Execute(scence);
    
    delete scence;
    return 0;
}