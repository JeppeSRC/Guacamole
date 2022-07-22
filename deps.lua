
VULKAN_SDK = os.getenv("VULKAN_SDK")
LD_LIBRARY_PATH = os.getenv("LD_LIBRARY_PATH")

IncludeDir = {}
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/include"
IncludeDir["glm"] = "libs/glm"

LibDir = {}
LibDir["Vulkan"] = "%{LD_LIBRARY_PATH}"

Lib = {}

filter "system:linux"

Lib["Vulkan"] = "vulkan"

filter ""