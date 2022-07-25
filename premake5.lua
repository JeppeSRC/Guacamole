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

project "Guacamole"
    kind "ConsoleApp"
    language "C++"
    location "build/"
    cppdialect "C++17"

    dependson "GLFW"

    pchheader "Guacamole.h"
    pchsource "src/Guacamole.cpp"

    filter "system:linux" 
    
        defines {
            "GM_LINUX",
            "VK_USE_PLATFORM_XLIB_KHR"
        }

        links {
            "dl",
            "pthread"
        }

    filter "system:windows"
        
        defines {
            "GM_WINDOWS",
            "VK_USE_PLATFORM_WIN32_KHR"
        }
            
    filter {}

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
