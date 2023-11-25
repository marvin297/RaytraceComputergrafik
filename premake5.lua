-- premake5.lua
workspace "MGRaytrace"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "MGRaytrace"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "MGRaytrace"