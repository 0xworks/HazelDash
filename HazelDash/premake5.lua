project "HazelDash"
	location "."  -- I'd rather this was in the Build/ directory.  But Hazel currently loads assets relative to the project dir. so that wont work.
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	flags
	{
		"MultiProcessorCompile"
	}

	configuration "vs*"
		buildoptions {
			"/permissive-",            -- Turn off MSVC permissive in order to catch C++ compliance issues
			"/FS"                      -- Fix "C1041 cannot open program database if multiple CL.EXE write to the same .PDB file"
		}
		linkoptions {
			"/NODEFAULTLIB:'libcmt'"   -- Fix "warning LNK4098: defaultlib 'libcmt.lib' conflicts with use of other libs" (note: premake broken? This option does not come out in the project)
		}

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING"
	}

	includedirs
	{
		"src",
		"../Hazel/Hazel/src",
		"../Hazel/Hazel/vendor/GLFW/include",
		"../Hazel/Hazel/vendor/entt/src/entt",
		"../Hazel/Hazel/vendor/glm",
		"../Hazel/Hazel/vendor/imgui",
		"../Hazel/Hazel/vendor/spdlog/include",
		"../Hazel/Hazel/vendor/entt/include"
	}
	
	links {
		"Hazel"
	}

	filter "system:windows"
		systemversion "latest"
		
	filter "configurations:Debug"
		defines "HZ_DEBUG"
		runtime "Debug"
		symbols "on"
        postbuildcommands {
           "{COPYDIR} ../Hazel/Hazel/vendor/VulkanSDK/bin/ \"%{cfg.targetdir}\""
        }

	filter "configurations:Profile"
		defines
		{
			"HZ_PROFILE",
			"TRACY_ENABLE"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"
