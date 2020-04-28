include "./premake_customizations.lua"

workspace "HazelDash"
	location ("Build/" .. _ACTION)
	architecture "x86_64"
	
	startproject "HazelDash"

	configurations
	{
		"Debug",
		"Release",
	}

	workspace_files 
	{
		".editorconfig"
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
	include "HazelAudio/HazelAudio"
	include "HazelAudio/HazelAudio/vendor/OpenAL-Soft"
	include "HazelAudio/HazelAudio/vendor/libogg"
	include "HazelAudio/HazelAudio/vendor/Vorbis"

group ""
	include "HazelDash"
