-- WalnutExternal.lua

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
LibraryDir = {}
Library = {}

if os.istarget("windows") then
   IncludeDir["glm"] = "../vendor/glm"
   IncludeDir["spdlog"] = "../vendor/spdlog/include"

   -- Vulkan SDK layout used on Windows
   IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
   LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
   Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
else
   -- Linux distributions ship glm, spdlog, the Vulkan headers and loader
   -- system-wide.
   IncludeDir["glm"] = "/usr/include"
   IncludeDir["spdlog"] = "/usr/include"
   IncludeDir["VulkanSDK"] = "/usr/include"
   Library["Vulkan"] = "vulkan"
end

group "Dependencies"
   include "vendor/imgui"
   include "vendor/glfw"
group ""

group "Core"
    include "Walnut/Build-Walnut.lua"

     
group ""