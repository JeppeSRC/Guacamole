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

    filter {}

include "deps.lua"

group "deps"
include "libs/glfw"

project "spdlog"
        kind "StaticLib"
        language "C++"
        location "build/"
        cppdialect "c++17"

        defines {
            "SPDLOG_COMPILED_LIB"
        }

        includedirs {
            "libs/spdlog/include"
        }

        files {
            "libs/spdlog/src/**.cpp"
        }

group ""

project "Guacamole"
    kind "ConsoleApp"
    language "C++"
    location "build/"
    cppdialect "C++17"

    dependson "GLFW"
    dependson "spdlog"

    pchheader "Guacamole.h"
    pchsource "src/Guacamole.cpp"

    defines {
        "SPDLOG_COMPILED_LIB"
    }

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
        "src/**.cpp",
        "src/**.h"
    }

    includedirs {
        "src/",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}"
    }

    libdirs {
        "%{LibDir.Vulkan}"
    }

    links {
        "spdlog",
        "GLFW",
        "%{Lib.Vulkan}"
    }

    