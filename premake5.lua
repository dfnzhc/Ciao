workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"
    startproject "Editor"

outputDir = "%{cfg.buildcfg}/%{prj.name}"

group "Dependencies"
group ""

characterset ("Unicode")

include "Ciao"
include "Editor"