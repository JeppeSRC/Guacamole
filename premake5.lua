workspace "Guacamole"
    configurations {"Debug", "Release"}
    architecture "x86_64"
    location "build"

    targetdir "build/bin/%{prj.name}/%{cfg.shortname}"
    objdir "build/obj/%{prj.name}/%{cfg.shortname}"

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

    dependson "spdlog"

    pchheader "Guacamole.h"
    pchsource "src/Guacamole.cpp"

    defines {
        "SPDLOG_COMPILED_LIB"
    }

    files {
        "src/**.cpp",
        "src/**.h"
    }

    filter "system:linux" 
    
        defines {
            "GM_LINUX",
            "VK_USE_PLATFORM_XCB_KHR",
        }

        links {
            "vulkan",
            "dl",
            "pthread",
            "xcb",
            "xcb-randr"
        }

        removefiles {
            "src/platform/windows/**.cpp",
            "src/platform/android/**.cpp"
        }

    filter "system:windows"
        
        defines {
            "GM_WINDOWS",
            "VK_USE_PLATFORM_WIN32_KHR",
            "_CRT_SECURE_NO_WARNINGS"
        }
        
        links {
            "vulkan-1"
        }

        removefiles {
            "src/platform/linux/**.cpp",
            "src/platform/android/**.cpp"
        }
        
    filter {}

    

    includedirs {
        "src/",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb}"
    }

    libdirs {
        "%{LibDir.Vulkan}"
    }

    links {
        "spdlog"
    }

    filter {"debug", "system:windows"}

        links {
            "spirv-cross-cored",
            "spirv-cross-glsld",
            "shaderc_sharedd"
        }

    filter {"release", "system:windows"}
        links {
            "spirv-cross-core",
            "spirv-cross-glsl",
            "shaderc_shared"
        }

    filter {"system:linux"}
        links {
            "spirv-cross-core",
            "spirv-cross-glsl",
            "shaderc_shared"
        }
        

    