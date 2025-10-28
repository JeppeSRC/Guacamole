newoption { 
    trigger = "window-system",
    value = "value",
    description = "Selects the window system on linux",
    allowed = {
        {"all", "Compiles with support for both XCB and Wayland"},
        {"wayland", "Wayland"},
        {"xcb", "XCB"},
    },

    default = "all"
}

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

        filter {"system:windows", "Debug"}

            buildoptions {
                "/MD"
            }

group ""

project "Guacamole"
-- All platforms
    kind "ConsoleApp"
    language "C++"
    location "build/"
    cppdialect "C++17"

    dependson "spdlog"

    pchheader "Guacamole.h"
    pchsource "src/Guacamole.cpp"

    defines {
        "SPDLOG_COMPILED_LIB",
    }

    files {
        "src/**.cpp",
        "src/**.h"
    }

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
 
    -- Linux 

    filter "system:linux" 

        buildoptions {
            "-Wall",
            "-Wno-reorder",
            "-Wno-misleading-indentation",
            "-mavx2",
            "-mfma"
        }
        
        defines {
            "GM_LINUX",
        }

        links {
            "vulkan",
            "dl",
            "pthread",
            "spdlog",
            "spirv-cross-core",
            "spirv-cross-glsl",
            "shaderc_shared"
        }
        
        removefiles {
            "src/Guacamole/platform/windows/**.cpp",
            "src/Guacamole/platform/android/**.cpp"
        }

    filter {"system:linux", "options:window-system=all or options:window-system=xcb"}
        defines {
            "VK_USE_PLATFORM_XCB_KHR",
            "GM_WINDOW_XCB"
        }

        links {
            "xcb",
            "xcb-randr",
            "xkbcommon",
            "xkbcommon-x11",
            "xcb-xfixes",
        }

    filter {"system:linux", "options:window-system=all or options:window-system=wayland"}
        defines {
            "VK_USE_PLATFORM_WAYLAND_KHR",
            "GM_WINDOW_WAYLAND"
        }

    -- Windows
    filter "system:windows"
        
        defines {
            "GM_WINDOWS",
            "VK_USE_PLATFORM_WIN32_KHR",
            "_CRT_SECURE_NO_WARNINGS"
        }
        
        links {
            "vulkan-1",
            "spdlog",
            "spirv-cross-core",
            "spirv-cross-glsl",
            "shaderc_shared"
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
    
    filter {"system:windows", "Debug"}
        buildoptions {
            "/MD"
        }        

    filter {}


   

    