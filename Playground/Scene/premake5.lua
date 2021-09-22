project "P6_Scene"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    targetdir ( "%{wks.location}/bin/" .. outputDir )
    objdir ( "%{prj.location}/obj/%{cfg.buildcfg}" )

	characterset ("Unicode")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"%{wks.location}/Ciao/src",
		"%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.FreeImage}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Stb}",
        "%{IncludeDir.Gli}",
	}

	links
	{
		"Ciao",
		"Glad",
		"%{Library.FreeImage}",
	}

	dpiawareness "HighPerMonitor"

	postbuildcommands
	{
		"{COPYDIR} \"%{LibraryDir.FreeImage_DLL}\" \"%{cfg.targetdir}\""
	}

    filter "system:windows"
		systemversion "latest"


    filter "configurations:Debug"
        defines "CIAO_DEBUG"
        symbols "On"
        runtime "Debug"

		postbuildcommands
		{
			"{COPYDIR} \"%{LibraryDir.Assimp_DLL_D}\" \"%{cfg.targetdir}\""
		}

        links
        {

        }

    filter "configurations:Release"
        defines "CIAO_RELEASE"
        optimize "On"
        runtime "Release"

		postbuildcommands
		{
			"{COPYDIR} \"%{LibraryDir.Assimp_DLL}\" \"%{cfg.targetdir}\""
		}

        links
        {
            
        }
