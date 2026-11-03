# chess-vulkun

A small Vulkan + Dear ImGui chess application built on top of the Walnut
framework.

## Building on Linux

The project uses CMake on Linux. Dependencies that are not vendored are
resolved from the system:

- Vulkan headers and loader (`vulkan-headers`, `vulkan-icd-loader`)
- GLM
- spdlog
- X11 development libraries (for the vendored GLFW build)

Dear ImGui, GLFW, and stb_image are vendored under `vendor/`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
```

Run it (the `run` target starts the executable from the directory that
contains the copied `img/` assets):

```sh
cmake --build build --target run
```

Or run the binary directly from its output directory:

```sh
cd build/bin && ./chess
```

### Release build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

## Editor support (clangd / Neovim)

CMake generates `build/compile_commands.json`, and a symlink at the repo
root points to it. clangd-based setups (including Neovim's built-in LSP)
will pick it up automatically:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

If the symlink is missing, recreate it with:

```sh
ln -sfn build/compile_commands.json compile_commands.json
```

## Building with Premake (optional)

The original Premake scripts are kept and now support Linux as well. If
`premake5` is available:

```sh
premake5 gmake2
make config=debug
```

## Windows

The existing Premake/MSVC workflow (`Build-Project.lua` / `Bruh.py`) is
unchanged. Run `premake5 vs2022` (or your Visual Studio version) and build
`chess.sln`.
