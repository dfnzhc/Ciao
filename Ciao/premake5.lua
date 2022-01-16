project "Ciao"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ( "%{wks.location}/bin/" .. outputDir )
    objdir ( "%{prj.location}/obj/%{cfg.buildcfg}" )

    pchheader "pch.h"
	pchsource "src/pch.cpp"
    
    files { 
        "src/**.h", 
        "src/**.cpp",
    }

    includedirs
	{
		"src",
        "%{IncDir.Glfw}",
        "%{IncDir.Glad}",
        "%{IncDir.Glm}",
        "%{IncDir.Spdlog}",
        "%{IncDir.ImGui}",
        "%{IncDir.ImGuizmo}",
        "%{IncDir.Stb}",
        "%{IncDir.Tinydir}",
        "%{IncDir.Tinygltf}",
        "%{IncDir.Tinyobjloader}",
	}

    links
    {
        "opengl32.lib",
        "Glad",
        "ImGui",
        "%{Lib.Glfw}",
    }
    

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

    
    -- 不使用预编译头的文件
    -- filter {}
    --     files 
    --     {

    --     }

    --     flags
    --     { 
    --         "NoPCH" 
    --     }

    filter "system:windows"
		systemversion "latest"
        defines "CIAO_PLATFORM_WINDOWS"

    filter "configurations:Debug"
        defines "CIAO_DEBUG"
        symbols "On"
        runtime "Debug"

        links
        {
        }

    filter "configurations:Release"
        defines "CIAO_RELEASE"
        optimize "On"
        runtime "Release"

        links
        {
        }