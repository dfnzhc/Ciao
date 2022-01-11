workspace "Ciao"
    configurations { "Debug", "Release" }
    architecture "x64"

outputDir = "%{cfg.buildcfg}/%{prj.name}"

group "Dependencies"
group ""

characterset ("Unicode")

include "Ciao"
include "Editor"