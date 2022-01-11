project "Ciao"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ( "%{wks.location}/bin/%{cfg.buildcfg}" )
    objdir ( "%{prj.location}/obj/%{cfg.buildcfg}" )

    files { 
        "src/**.h", 
        "src/**.cpp",
    }

    includedirs
	{
		"src",
	}

    links
    {
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
        }

    filter "configurations:Release"
        defines "CIAO_RELEASE"
        optimize "On"
        runtime "Release"

        links
        {
        }