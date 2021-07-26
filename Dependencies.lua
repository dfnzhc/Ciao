
IncludeDir = {}
IncludeDir["SDL2"]          = "%{wks.location}/Ciao/Deps/sdl2/include"
IncludeDir["Glad"]          = "%{wks.location}/Ciao/Deps/glad/include"
IncludeDir["glm"]           = "%{wks.location}/Ciao/Deps/glm"
IncludeDir["spdlog"]        = "%{wks.location}/Ciao/Deps/spdlog/include"
IncludeDir["FreeImage"]     = "%{wks.location}/Ciao/Deps/FreeImage/include"
IncludeDir["ImGui"]         = "%{wks.location}/Ciao/Deps/imgui"
IncludeDir["Assimp"]        = "%{wks.location}/Ciao/Deps/assimp/include"
IncludeDir["Stb"]           = "%{wks.location}/Ciao/Deps/stb"
IncludeDir["Gli"]           = "%{wks.location}/Ciao/Deps/gli"


LibraryDir = {}
LibraryDir["SDL2"]          = "%{wks.location}/Ciao/Deps/sdl2/lib"
LibraryDir["FreeImage"]     = "%{wks.location}/Ciao/Deps/FreeImage/lib"
LibraryDir["FreeImage_DLL"] = "%{wks.location}/Ciao/Deps/FreeImage/bin"

LibraryDir["Assimp"]        = "%{wks.location}/Ciao/Deps/assimp/lib"
LibraryDir["Assimp_DLL"]    = "%{wks.location}/Ciao/Deps/assimp/bin/Release"
LibraryDir["Assimp_DLL_D"]  = "%{wks.location}/Ciao/Deps/assimp/bin/Debug"


Library = {}
Library["SDL2"]             = "%{LibraryDir.SDL2}/SDL2.lib"
Library["FreeImage"]        = "%{LibraryDir.FreeImage}/FreeImage.lib"
Library["Assimp"]           = "%{LibraryDir.Assimp}/Release/assimp.lib"
Library["Assimp_Debug"]     = "%{LibraryDir.Assimp}/Debug/assimpd.lib"
