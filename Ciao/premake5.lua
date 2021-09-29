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
		-- "src/Asset/",
		-- "src/Core/",
		-- "src/Renderer/",
		-- "src/Utils/",
        -- "%{IncludeDir.glfw}",
        -- "%{IncludeDir.Glad}",
        -- "%{IncludeDir.glm}",
        -- "%{IncludeDir.spdlog}",
        -- "%{IncludeDir.ImGui}",
        -- "%{IncludeDir.Assimp}",
        -- "%{IncludeDir.Stb}",
        -- "%{IncludeDir.Gli}",
        -- "%{IncludeDir.MeshOptimizer}",
        -- "%{IncludeDir.rapidjson}",
	}

    links
    {
        "opengl32.lib",
        "%{Library.glfw}",
        -- "Glad",
        -- "ImGui",
        -- "MeshOptimizer",
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