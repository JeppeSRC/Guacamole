workspace "Guacamole"
    configurations {"Debug", "Release"}
    architecture "x86_64"
    location "build"

    targetdir "build/bin/%{prj.name}"
    objdir "build/obj/%{prj.name}"

	flags {
		"MultiProcessorCompile"
	}


    filter "Debug"

        defines {
            "GM_DEBUG"
        }
        
        symbols "On"

    filter "Release"

        defines {
            "GM_RELEASE"
        }

        optimize "On"

    filter ""

include "deps.lua"

group "deps"
include "libs/glfw"
group ""

project "Gucamole"
    kind "ConsoleApp"
    language "C++"
    location "build/"
    cppdialect "C++17"

    files {
        "src/**.cpp"
    }

    includedirs {
        "src/",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.entt}"
    }

    libdirs {
        "%{LibDir.Vulkan}"
    }

    links {
        "GLFW",
        "%{Lib.Vulkan}"
    }
