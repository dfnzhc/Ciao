#pragma once

#include "Application.h"
#include "Core/Scence.h"
#include "Core/Log.h"

extern Ciao::Scence* Ciao::CreateScence();

int main()
{
    Ciao::Log::Init();
    
    Ciao::Scence* scence = Ciao::CreateScence();
    Ciao::Application::GetAppInst().Execute(scence);
    
    delete scence;
    return 0;
}