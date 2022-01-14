workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Editor"

outputDir = "%{cfg.buildcfg}/%{prj.name}"

include "Dependencies.lua"

group "Dependencies"
    include "Deps/glad"
    include "Deps/imgui"
group ""

characterset ("Unicode")

include "Ciao"
include "Playground/Sandbox"