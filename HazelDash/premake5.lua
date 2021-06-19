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

	-- From VS2019 16.9.0 onwards, multiprocessor compile can cause
	-- error "C1041 cannot open program database if multiple CL.EXE write to the same .PDB file, please use /FS"
	-- There are two solutions:
	-- Either:  Add /FS, as I have done here (for VS builds only)
	-- Or:      Do not use multiprocessor build in conjuction with MTT (https://devblogs.microsoft.com/cppblog/improved-parallelism-in-msbuild/)
	--
	-- I have gone with the former because MTT is better, and I dont want to disable it.
	configuration "vs*"
		buildoptions {
		"/permissive-",
		"/FS"
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
           "{COPY} ../Hazel/Hazel/vendor/VulkanSDK/bin/ %{cfg.targetdir}"
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
