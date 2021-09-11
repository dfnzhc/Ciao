include "Dependencies.lua"

workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"


outputDir = "%{cfg.buildcfg}/%{prj.name}"

group "Dependencies"
    include "Ciao/Deps/glad"
    include "Ciao/Deps/imgui"
group ""

characterset ("Unicode")

include "Ciao"
include "Playground/Sandbox"
include "Playground/VtxPulling"
include "Playground/Shadow"