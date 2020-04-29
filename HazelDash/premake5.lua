project "HazelDash"
	location "."  -- I'd rather this was in the Build/ directory.  But Hazel currently loads assets relative to the project dir. so that wont work.
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"../Hazel/Hazel/src",
		"../Hazel/Hazel/vendor/GLFW/include",
		"../Hazel/Hazel/vendor/glm",
		"../Hazel/Hazel/vendor/imgui",
		"../Hazel/Hazel/vendor/spdlog/include"
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

	filter "configurations:Release"
		defines "HZ_RELEASE"
		runtime "Release"
		optimize "on"
