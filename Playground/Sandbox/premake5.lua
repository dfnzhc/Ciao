project "Sandbox"
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
        "%{IncDir.Glfw}",
        "%{IncDir.Glad}",
        "%{IncDir.Glm}",
        "%{IncDir.Spdlog}",
        "%{IncDir.ImGui}",
        "%{IncDir.ImGuizmo}",
        "%{IncDir.Stb}",
        "%{IncDir.Tinydir}",
        "%{IncDir.Tbb}",
        "%{IncDir.Meshoptimizer}",
	}

	links
	{
		"Ciao",
	}

	dpiawareness "HighPerMonitor"

	-- postbuildcommands
	-- {
	-- 	"{COPYDIR} \"%{Lib.SDL2_DLL}\" \"%{cfg.targetdir}\""
	-- }

    filter "system:windows"
		systemversion "latest"


    filter "configurations:Debug"
        defines "CIAO_DEBUG"
        symbols "On"
        runtime "Debug"
		

		postbuildcommands
		{
			"{COPYDIR} \"%{Lib.Tbb12_dll_d}\" \"%{cfg.targetdir}\"",
			"{COPYDIR} \"%{Lib.Assimp_dll_d}\" \"%{cfg.targetdir}\"",
		}

        links
        {
            "%{Lib.Tbb_d}",
            "%{Lib.Tbb12_d}",
            "%{Lib.Assimp_d}",
        }

    filter "configurations:Release"
        defines "CIAO_RELEASE"
        optimize "On"
        runtime "Release"

		postbuildcommands
		{
			"{COPYDIR} \"%{Lib.Tbb12_dll}\" \"%{cfg.targetdir}\"",
			"{COPYDIR} \"%{Lib.Assimp_dll}\" \"%{cfg.targetdir}\"",
		}

        links
        {
            "%{Lib.Tbb}",
            "%{Lib.Tbb12}",
            "%{Lib.Assimp}",
        }
