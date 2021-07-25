project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    targetdir ( "%{wks.location}/bin/" .. outputDir )
    objdir ( "%{wks.location}/bin-int/" .. outputDir )

	files
	{
		"src/**.h",
		"src/**.cpp",

        "Deps/glm/glm/**.hpp",
		"Deps/glm/glm/**.inl",
		"%{IncludeDir.Stb}/stb_image.h",
		"%{IncludeDir.Stb}/stb_image_write.h",
	}

	includedirs
	{
		"%{wks.location}/Ciao/src",
		"%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.FreeImage}",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.Stb}",
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
