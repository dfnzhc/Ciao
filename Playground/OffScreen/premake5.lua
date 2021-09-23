project "P7_OffScreen"
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
		"%{IncludeDir.Glad}",
		"%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.Stb}",
        "%{IncludeDir.Gli}",
	}

	links
	{
		"Ciao",
		"Glad",
		"%{Library.glfw}",
	}

	dpiawareness "HighPerMonitor"

	-- postbuildcommands
	-- {
	-- 	"{COPYDIR} \"%{LibraryDir.FreeImage_DLL}\" \"%{cfg.targetdir}\""
	-- }

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
