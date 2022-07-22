workspace "Guacamole"
    configurations {"Debug", "Release"}
    architecture "x86_64"
    location "build"

    targetdir "build/bin"


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

project "Gucamole"
    kind "ConsoleApp"
    language "C++"
    location "build/"

    files {
        "src/**.cpp"
    }

    includedirs {
        "src/",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glm}"
    }

    libdirs {
        "%{LibDir.Vulkan}"
    }

    links {
        "%{Lib.Vulkan}"
    }
