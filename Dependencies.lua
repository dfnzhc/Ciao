DepsDir = {}
DepsDir["Glfw"]          = "%{wks.location}/Deps/glfw"
DepsDir["Glad"]          = "%{wks.location}/Deps/glad"
DepsDir["Glm"]           = "%{wks.location}/Deps/glm"
DepsDir["Spdlog"]        = "%{wks.location}/Deps/spdlog"
DepsDir["ImGui"]         = "%{wks.location}/Deps/imgui"
DepsDir["ImGuizmo"]      = "%{wks.location}/Deps/ImGuizmo"
DepsDir["Stb"]           = "%{wks.location}/Deps/stb"
DepsDir["Tinydir"]       = "%{wks.location}/Deps/tinydir"
DepsDir["Tinygltf"]      = "%{wks.location}/Deps/tinygltf"
DepsDir["Tinyobjloader"] = "%{wks.location}/Deps/tinyobjloader"
DepsDir["Tbb"]           = "%{wks.location}/Deps/tbb"

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


LibDir = {}
LibDir["Glfw"]          = "%{DepsDir.Glfw}/lib-vc2022"
LibDir["Tbb"]           = "%{DepsDir.Tbb}/lib/intel64/vc14"
LibDir["Tbb_DLL"]       = "%{DepsDir.Tbb}/redist/intel64/vc14"


Lib = {}
Lib["Glfw"]             = "%{LibDir.Glfw}/glfw3.lib"
Lib["Tbb"]              = "%{LibDir.Tbb}/tbb.lib"
Lib["Tbb_d"]            = "%{LibDir.Tbb}/tbb_debug.lib"

Lib["Tbb12"]            = "%{LibDir.Tbb}/tbb12.lib"
Lib["Tbb12_dll"]        = "%{LibDir.Tbb_DLL}/tbb12.dll"

Lib["Tbb12_d"]          = "%{LibDir.Tbb}/tbb12_debug.lib"
Lib["Tbb12_dll_d"]      = "%{LibDir.Tbb_DLL}/tbb12_debug.dll"
