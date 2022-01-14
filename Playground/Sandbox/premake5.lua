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
        "%{IncDir.SDL2}",
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
		"Ciao",
		--"%{Library.glfw}",
		--"Glad",
	}

	dpiawareness "HighPerMonitor"

	postbuildcommands
	{
		"{COPYDIR} \"%{Lib.SDL2_DLL}\" \"%{cfg.targetdir}\""
	}

    filter "system:windows"
		systemversion "latest"


    filter "configurations:Debug"
        defines "CIAO_DEBUG"
        symbols "On"
        runtime "Debug"
		

		postbuildcommands
		{
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
		}

        links
        {
            
        }
