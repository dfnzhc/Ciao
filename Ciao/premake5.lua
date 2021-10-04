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
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.Stb}",
        "%{IncludeDir.Gli}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.MeshOptimizer}",
        "%{IncludeDir.rapidjson}",
        -- "%{IncludeDir.ImGui}",
	}

    links
    {
        "opengl32.lib",
        "Glad",
        "%{Library.glfw}",
        "MeshOptimizer",
        -- "ImGui",
    }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}
    
    -- 不使用预编译头的文件
    -- filter "files:vendor/ImGuizmo/**.cpp"
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
            "%{Library.Assimp_Debug}",
        }

    filter "configurations:Release"
        defines "CIAO_RELEASE"
        optimize "On"
        runtime "Release"

        links
        {
            "%{Library.Assimp}",
        }