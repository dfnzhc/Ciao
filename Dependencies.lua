DepsDir = {}
DepsDir["Glfw"]          = "%{wks.location}/Deps/glfw"
DepsDir["Glad"]          = "%{wks.location}/Deps/glad"
DepsDir["Glm"]           = "%{wks.location}/Deps/glm"
DepsDir["Spdlog"]        = "%{wks.location}/Deps/spdlog"
DepsDir["ImGui"]         = "%{wks.location}/Deps/imgui"
DepsDir["ImGuizmo"]      = "%{wks.location}/Deps/ImGuizmo"
DepsDir["Stb"]           = "%{wks.location}/Deps/stb"
DepsDir["Tinydir"]       = "%{wks.location}/Deps/tinydir"
DepsDir["Tbb"]           = "%{wks.location}/Deps/tbb"
DepsDir["Meshoptimizer"] = "%{wks.location}/Deps/meshoptimizer"
DepsDir["Assimp"]        = "%{wks.location}/Deps/assimp"

IncDir = {}
IncDir["Glfw"]          = "%{DepsDir.Glfw}/include"
IncDir["Glad"]          = "%{DepsDir.Glad}/include"
IncDir["Glm"]           = "%{DepsDir.Glm}"
IncDir["Spdlog"]        = "%{DepsDir.Spdlog}/include"
IncDir["ImGui"]         = "%{DepsDir.ImGui}"
IncDir["ImGuizmo"]      = "%{DepsDir.ImGuizmo}"
IncDir["Stb"]           = "%{DepsDir.Stb}"
IncDir["Tinydir"]       = "%{DepsDir.Tinydir}"
IncDir["Tinygltf"]      = "%{DepsDir.Tinygltf}"
IncDir["Tinyobjloader"] = "%{DepsDir.Tinyobjloader}"
IncDir["Tbb"]           = "%{DepsDir.Tbb}/include"
IncDir["Meshoptimizer"] = "%{DepsDir.Meshoptimizer}/src"
IncDir["Assimp"]        = "%{DepsDir.Assimp}/include"


LibDir = {}
LibDir["Glfw"]          = "%{DepsDir.Glfw}/lib-vc2022"
LibDir["Tbb"]           = "%{DepsDir.Tbb}/lib/intel64/vc14"
LibDir["Tbb_DLL"]       = "%{DepsDir.Tbb}/redist/intel64/vc14"

LibDir["Assimp"]        = "%{wks.location}/Deps/assimp/lib"
LibDir["Assimp_DLL"]    = "%{wks.location}/Deps/assimp/bin"


Lib = {}
Lib["Glfw"]             = "%{LibDir.Glfw}/glfw3.lib"
Lib["Tbb"]              = "%{LibDir.Tbb}/tbb.lib"
Lib["Tbb_d"]            = "%{LibDir.Tbb}/tbb_debug.lib"

Lib["Tbb12"]            = "%{LibDir.Tbb}/tbb12.lib"
Lib["Tbb12_dll"]        = "%{LibDir.Tbb_DLL}/tbb12.dll"

Lib["Tbb12_d"]          = "%{LibDir.Tbb}/tbb12_debug.lib"
Lib["Tbb12_dll_d"]      = "%{LibDir.Tbb_DLL}/tbb12_debug.dll"


Lib["Assimp"]           = "%{LibDir.Assimp}/Release/assimp.lib"
Lib["Assimp_dll"]       = "%{LibDir.Assimp_DLL}/Release/assimp.dll"

Lib["Assimp_d"]         = "%{LibDir.Assimp}/Debug/assimpd.lib"
Lib["Assimp_dll_d"]     = "%{LibDir.Assimp_DLL}/Debug/assimpd.dll"
