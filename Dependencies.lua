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


LibDir = {}
LibDir["Glfw"]          = "%{DepsDir.Glfw}/lib-vc2022"


Lib = {}
Lib["Glfw"]             = "%{LibDir.Glfw}/glfw3.lib"
