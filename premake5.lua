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
        "SPDLOG_COMPILED_LIB",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
    }

    files {
        "src/**.cpp",
        "src/**.h"
    }

    filter "system:linux" 

        buildoptions {
            "-Wall",
            "-Wno-reorder",
            "-Wno-misleading-indentation",
            "-mavx2"
        }
        
        linkoptions {
        }

        defines {
            "GM_LINUX",
            "VK_USE_PLATFORM_XCB_KHR",
        }

        links {
            "vulkan",
            "dl",
            "pthread",
            "xcb",
            "xcb-randr",
            "xkbcommon",
            "xkbcommon-x11",
            "xcb-xfixes"
        }

        removefiles {
            "src/Guacamole/platform/windows/**.cpp",
            "src/Guacamole/platform/android/**.cpp"
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
            "src/Guacamole/platform/linux/**.cpp",
            "src/Guacamole/platform/android/**.cpp"
        }

        filter {"system:windows", "Release"}
            buildoptions {
                "/Ob2",
                "/Ot",
                "/Oy",
                "/arch:AVX2"
            }        
    filter {}

    

    includedirs {
        "src/",
        "%{IncludeDir.Vulkan}",
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

    filter {"Debug", "system:windows"}

        links {
            "spirv-cross-cored",
            "spirv-cross-glsld",
            "shaderc_sharedd"
        }

    filter {"Release", "system:windows"}
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
        

    