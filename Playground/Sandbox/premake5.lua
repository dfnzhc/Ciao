project "P0_Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	debugdir ( "%{wks.location}" )

    targetdir ( "%{wks.location}/bin/" .. outputDir )
    objdir ( "%{prj.location}/obj/%{cfg.buildcfg}" )

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
        "%{IncludeDir.glfw}",
		"%{IncludeDir.Glad}",
        "%{IncludeDir.Stb}",
        "%{IncludeDir.Gli}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.ImGui}",
	}

	links
	{
		"Ciao",
		--"%{Library.glfw}",
		--"Glad",
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
