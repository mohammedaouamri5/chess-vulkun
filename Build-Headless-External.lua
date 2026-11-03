-- WalnutExternal-Headless.lua

IncludeDir = {}
if os.istarget("windows") then
   IncludeDir["glm"] = "../vendor/glm"
   IncludeDir["spdlog"] = "../vendor/spdlog/include"
else
   IncludeDir["glm"] = "/usr/include"
   IncludeDir["spdlog"] = "/usr/include"
end

group "Dependencies"
group ""

group "Core"
    include "Walnut/Build-Walnut-Headless.lua"

     
group ""