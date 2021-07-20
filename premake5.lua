include "Dependencies.lua"

workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"


outputDir = "%{cfg.buildcfg}/%{prj.name}"

group "Dependencies"
    include "Ciao/Deps/glad"
    include "Ciao/Deps/imgui"
group ""

include "Ciao"
include "Playground/Sandbox"