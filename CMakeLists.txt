cmake_minimum_required(VERSION 3.20)

project(chess LANGUAGES C CXX)

# Generate compile_commands.json for clangd / editor language servers.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build the vendored libraries (GLFW, Dear ImGui, Walnut) as shared objects
# instead of static archives. Set to OFF for a fully static app build.
option(BUILD_SHARED_LIBS "Build libraries as shared libraries" ON)

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

# Keep the shared-library lookup relative to the executable so the build
# tree can be moved without changing the RUNPATH.
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

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
add_library(imgui
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

add_library(Walnut ${WALNUT_SOURCES})

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
# Chess logging (quill, vendored — usage in chess/src/Log.hpp)
# ---------------------------------------------------------------------------

add_subdirectory(vendor/quill EXCLUDE_FROM_ALL)

# One small shared object that compiles all of quill's machinery; the chess
# sources only depend on it through ChessLog.hpp / chess/src/Log.hpp.
add_library(quill_backend SHARED
    vendor/quill_backend/src/ChessLog.cpp
)
target_include_directories(quill_backend
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/quill_backend/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/chess/src
)
target_link_libraries(quill_backend PUBLIC quill::quill)

# Any -DLOG_<CHANNEL>=1 flag given at configure time is turned into a compile
# definition automatically, so adding a channel needs no CMake changes.
# Explicit flags are normalized to 1/0 (so `-DLOG_BORD=ON` works too).
get_cmake_property(CHESSLOG_CACHE_VARS CACHE_VARIABLES)
set(CHESSLOG_DEFINES "")
foreach(_var IN LISTS CHESSLOG_CACHE_VARS)
    if(_var MATCHES "^LOG_" OR _var STREQUAL "CHESS_LOG_COLORS")
        set(_val "${${_var}}")
        if(_val)
            list(APPEND CHESSLOG_DEFINES "${_var}=1")
        else()
            list(APPEND CHESSLOG_DEFINES "${_var}=0")
        endif()
    endif()
endforeach()
if(CHESSLOG_DEFINES)
    target_compile_definitions(quill_backend PRIVATE ${CHESSLOG_DEFINES})
endif()

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
target_link_libraries(chess PRIVATE Walnut quill_backend)
target_compile_definitions(chess PRIVATE WL_PLATFORM_LINUX)
if(CHESSLOG_DEFINES)
    target_compile_definitions(chess PRIVATE ${CHESSLOG_DEFINES})
endif()

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

# Convenience target: `cmake --build build --target flags` — lists all available
# logging channels / presets / usefull build commands in one place.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/chess/src/Log.hpp" CHESSLOG_HEADER_SRC)
string(REGEX MATCHALL "F\\([A-Za-z0-9_]+," CHESSLOG_ENTRIES "${CHESSLOG_HEADER_SRC}")
set(CHESSLOG_FLAG_CMDS
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "logging channels (from chess/src/Log.hpp):"
)
foreach(_e IN LISTS CHESSLOG_ENTRIES)
    string(REGEX REPLACE "^F\\(([A-Za-z0-9_]+)," "\\1" _name "${_e}")
    if(NOT _name STREQUAL "NAME") # skip the docstring example in Log.hpp
        list(APPEND CHESSLOG_FLAG_CMDS COMMAND ${CMAKE_COMMAND} -E echo "   -DLOG_${_name}=1")
    endif()
endforeach()
list(APPEND CHESSLOG_FLAG_CMDS
    COMMAND ${CMAKE_COMMAND} -E echo "   -DCHESS_LOG_COLORS=0        (strip ANSI colours)"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "all-in-one presets (CMakePresets.json):"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --list-presets"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake --preset logs-all && cmake --build build --target run"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "clear cached flags:"
    COMMAND ${CMAKE_COMMAND} -E echo "   cmake -B build -U 'LOG_*' -U CHESS_LOG_COLORS"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)

add_custom_target(flags ${CHESSLOG_FLAG_CMDS} VERBATIM)
