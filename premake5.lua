include "Dependencies.lua"

workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"


outputDir = "%{cfg.buildcfg}/%{prj.name}"

group "Dependencies"
    include "Ciao/Deps/glad"
    include "Ciao/Deps/imgui"
    include "Ciao/Deps/meshoptimizer"
group ""

characterset ("Unicode")

include "Ciao"
include "Playground/Sandbox"
include "Playground/VtxPulling"
include "Playground/Shadow"
include "Playground/Tessellation"
include "Playground/MeshDraw"
include "Playground/PBR"