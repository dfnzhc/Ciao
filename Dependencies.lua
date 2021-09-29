IncludeDir = {}
IncludeDir["glfw"]          = "%{wks.location}/Deps/glfw/include"
IncludeDir["Glad"]          = "%{wks.location}/Deps/glad/include"
IncludeDir["glm"]           = "%{wks.location}/Deps/glm"
IncludeDir["spdlog"]        = "%{wks.location}/Deps/spdlog/include"
IncludeDir["ImGui"]         = "%{wks.location}/Deps/imgui"
IncludeDir["Assimp"]        = "%{wks.location}/Deps/assimp/include"
IncludeDir["Stb"]           = "%{wks.location}/Deps/stb"
IncludeDir["Gli"]           = "%{wks.location}/Deps/gli"
IncludeDir["MeshOptimizer"] = "%{wks.location}/Deps/meshoptimizer/src"
IncludeDir["rapidjson"]     = "%{wks.location}/Deps/rapidjson/include"


LibraryDir = {}
LibraryDir["glfw"]          = "%{wks.location}/Deps/glfw/lib-vc2019"

LibraryDir["Assimp"]        = "%{wks.location}/Deps/assimp/lib"
LibraryDir["Assimp_DLL"]    = "%{wks.location}/Deps/assimp/bin/Release"
LibraryDir["Assimp_DLL_D"]  = "%{wks.location}/Deps/assimp/bin/Debug"


Library = {}
Library["glfw"]             = "%{LibraryDir.glfw}/glfw3.lib"
Library["Assimp"]           = "%{LibraryDir.Assimp}/Release/assimp.lib"
Library["Assimp_Debug"]     = "%{LibraryDir.Assimp}/Debug/assimpd.lib"
