project "Ciao"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ( "%{wks.location}/bin/" .. outputDir )
    objdir ( "%{wks.location}/bin-int/" .. outputDir )

    pchheader "pch.h"
	pchsource "src/pch.cpp"

    files { 
        "src/**.h", 
        "src/**.cpp",
        "%{IncludeDir.glm}/**.hpp",
		"%{IncludeDir.glm}/**.inl",
		"%{IncludeDir.Stb}/stb_image.h",
		"%{IncludeDir.Stb}/stb_image_write.h",
    }

    includedirs
	{
		"src",
		"src/Asset/",
		"src/Core/",
		"src/Renderer/",
		"src/Util/",
        "%{IncludeDir.SDL2}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.FreeImage}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.Stb}",
	}

    links
    {
        "opengl32.lib",
        "Glad",
        "ImGui",
        "%{Library.SDL2}",
        "%{Library.FreeImage}",
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