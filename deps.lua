
VULKAN_SDK = os.getenv("VULKAN_SDK")
LD_LIBRARY_PATH = "%{VULKAN_SDK}/Lib"

IncludeDir = {}
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/include"
IncludeDir["glm"] = "libs/glm"
IncludeDir["glfw"] = "libs/glfw/include"
IncludeDir["entt"] = "libs/entt/single_include"
IncludeDir["spdlog"] = "libs/spdlog/include"

LibDir = {}
LibDir["Vulkan"] = "%{LD_LIBRARY_PATH}"

Lib = {}

filter "system:linux"

Lib["Vulkan"] = "vulkan-1"

filter {}