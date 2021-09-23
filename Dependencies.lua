IncludeDir = {}
IncludeDir["glfw"]          = "%{wks.location}/Ciao/Deps/glfw/include"
IncludeDir["Glad"]          = "%{wks.location}/Ciao/Deps/glad/include"
IncludeDir["glm"]           = "%{wks.location}/Ciao/Deps/glm"
IncludeDir["spdlog"]        = "%{wks.location}/Ciao/Deps/spdlog/include"
IncludeDir["ImGui"]         = "%{wks.location}/Ciao/Deps/imgui"
IncludeDir["Assimp"]        = "%{wks.location}/Ciao/Deps/assimp/include"
IncludeDir["Stb"]           = "%{wks.location}/Ciao/Deps/stb"
IncludeDir["Gli"]           = "%{wks.location}/Ciao/Deps/gli"
IncludeDir["MeshOptimizer"] = "%{wks.location}/Ciao/Deps/meshoptimizer/src"
IncludeDir["rapidjson"]     = "%{wks.location}/Ciao/Deps/rapidjson/include"


LibraryDir = {}
LibraryDir["glfw"]          = "%{wks.location}/Ciao/Deps/glfw/lib-vc2019"

LibraryDir["Assimp"]        = "%{wks.location}/Ciao/Deps/assimp/lib"
LibraryDir["Assimp_DLL"]    = "%{wks.location}/Ciao/Deps/assimp/bin/Release"
LibraryDir["Assimp_DLL_D"]  = "%{wks.location}/Ciao/Deps/assimp/bin/Debug"


Library = {}
Library["glfw"]             = "%{LibraryDir.glfw}/glfw3.lib"
Library["Assimp"]           = "%{LibraryDir.Assimp}/Release/assimp.lib"
Library["Assimp_Debug"]     = "%{LibraryDir.Assimp}/Debug/assimpd.lib"
