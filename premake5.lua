include "Hazel/vendor/premake/premake_customization/solution_items.lua"

workspace "HazelDash"
	location ("Build/" .. _ACTION)
	architecture "x86_64"
	
	startproject "HazelDash"

	configurations
	{
		"Debug",
		"Profile",
		"Release"
	}

	solution_items
	{
		"../../.editorconfig",
		"../../README.md"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Hazel/Hazel"
	include "Hazel/Hazel/vendor/GLFW"
	include "Hazel/Hazel/vendor/Glad"
	include "Hazel/Hazel/vendor/imgui"	
	include "Hazel/Hazel/vendor/yaml-cpp"	

group ""
	include "HazelDash"
