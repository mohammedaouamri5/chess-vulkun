-- premake5.lua
workspace "chess"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "chess"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Build-External.lua"
include "chess/Build-chess.lua"