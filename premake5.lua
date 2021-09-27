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
include "Playground/Shadow"
include "Playground/MeshDraw"
include "Playground/PBR"
include "Playground/Scene"
include "Playground/OffScreen"
include "Playground/SSAO"
include "Playground/HDR"
include "Playground/MergeScene"
include "Playground/CullingCPU"