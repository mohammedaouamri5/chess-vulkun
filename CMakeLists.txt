cmake_minimum_required(VERSION 3.20)

project(chess LANGUAGES C CXX)

# Generate compile_commands.json for clangd / editor language servers.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The Walnut runtime (which this project vendors) requires C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to a Debug build for single-config generators (Makefiles/Ninja).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

# GLFW: use the vendored fork (it provides the custom-titlebar API used by
# Walnut's GUI platform layer).
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND  OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11      ON  CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)

# Dear ImGui (vendored, matching the API used by Walnut).
add_library(imgui STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_demo.cpp
    Walnut/Platform/GUI/Walnut/ImGui/ImGuiBuild.cpp
)
target_include_directories(imgui PUBLIC
    vendor/imgui
    vendor/imgui/backends
    vendor/glfw/include
)
target_link_libraries(imgui PUBLIC glfw Vulkan::Vulkan)
target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

# ---------------------------------------------------------------------------
# Walnut runtime
# ---------------------------------------------------------------------------

file(GLOB_RECURSE WALNUT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Walnut/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Walnut/Source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Walnut/Platform/GUI/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Walnut/Platform/GUI/*.h"
)
# ImGuiBuild.cpp is compiled as part of the imgui target above.
list(FILTER WALNUT_SOURCES EXCLUDE REGEX "ImGuiBuild\\.cpp$")

add_library(Walnut STATIC ${WALNUT_SOURCES})

target_include_directories(Walnut PUBLIC
    Walnut/Source
    Walnut/Platform/GUI
    vendor/imgui
    vendor/imgui/backends
    vendor/glfw/include
    vendor/stb_image
)

target_link_libraries(Walnut PUBLIC
    imgui
    glfw
    Vulkan::Vulkan
    glm::glm
    spdlog::spdlog
)

target_compile_definitions(Walnut PUBLIC WL_PLATFORM_LINUX)

# ---------------------------------------------------------------------------
# Chess application
# ---------------------------------------------------------------------------

add_executable(chess
    chess/src/Bord.cpp
    chess/src/Clik.cpp
    chess/src/Move.cpp
    chess/src/RBord.cpp
    chess/src/Vector.cpp
    chess/src/WalnutApp.cpp
)

target_include_directories(chess PRIVATE chess/src)
target_link_libraries(chess PRIVATE Walnut)
target_compile_definitions(chess PRIVATE WL_PLATFORM_LINUX)

# Map the CMake build type onto Walnut's configuration defines.
target_compile_definitions(Walnut PUBLIC
    $<$<CONFIG:Debug>:WL_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:WL_RELEASE>
)
target_compile_definitions(chess PRIVATE
    $<$<CONFIG:Debug>:WL_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:WL_RELEASE>
)

# The application loads its piece textures from a path relative to the
# working directory, so keep a copy next to the executable.
add_custom_command(TARGET chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/chess/img"
            "$<TARGET_FILE_DIR:chess>/img"
    COMMENT "Copying chess assets"
)

# Convenience target: `cmake --build build --target run`
add_custom_target(run
    COMMAND chess
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:chess>"
    DEPENDS chess
    USES_TERMINAL
)
