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

### Shared libraries

GLFW, Dear ImGui, and Walnut are built as shared objects by default:

- `build/lib/libglfw.so`
- `build/lib/libimgui.so`
- `build/lib/libWalnut.so`

The executable links against them from `build/lib`. Its RUNPATH is relative
to the binary (`$ORIGIN/../lib`), so the build tree can be moved as a whole.

To build static libraries instead:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF
cmake --build build -j"$(nproc)"
```

### Release build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

## Logging

The app has a compile-time-switched logging system (built on vendored
[quill](https://github.com/odygrd/quill)); the full guide lives in the header
comment of `chess/src/Log.hpp`.

Write logs with the builder API — every `.cpp` sets its channel first:

```cpp
#define CHESSLOG_MODULE BORD   // RENDERING / BORD / ENGINE
#include "Log.hpp"

LOG_INFO("en-passant check")
    .with_variable("right", CORD + DIR::RIGHT)
    .with_variable("left",  CORD + DIR::LEFT)
    .log();      // .log() is optional: the temporary flushes in its destructor

LOG_WARN("king unsafe, removing move").with_variable("to", TO).log();
LOG_ERR("...").log();
 ```

Channels are switched at configure time; a disabled channel's calls compile to
`while (false) ...` — zero bytes, the arguments are never evaluated. With no
flag at all nothing logs and no backend thread is started:

```sh
cmake -S . -B build -DLOG_BORD=1 -DLOG_RENDERING=1   # or:
cmake --preset logs-bord     # logs-rendering / logs-bord / logs-engine / logs-all
cmake --build build -j"$(nproc)"
```

`CHESS_LOG_COLORS=0` strips the ANSI colours of the `[tag]` prefix (colours are
on by default). Output format:

```
04:42:08.089 [bord] Clik.cpp:44 INFO pick piece square={2, 6}
```

`with_variable` draws ints, floats, bools, text, `Vector` (`{x, y}`),
`std::list<Vector>` and `Bord` (8×8 ASCII dump) with no heap allocation;
anything else is a link error telling you to add a `ChessLog_print()` overload.
For engine hot loops there are also raw passthrough macros:
`LOG_INFO_RAW("fmt {}", args)` / `LOG_WARN_RAW(...)` / `LOG_ERR_RAW(...)`.

Adding a new channel is one line in the `CHESSLOG_CHANNELS` table in
`chess/src/Log.hpp` plus `#define CHESSLOG_MODULE NAME` in the files that use
it; the `-DLOG_NAME=1` flag then works automatically with no CMake changes.

The logging machinery itself lives in `vendor/quill_backend/` and builds to
`build/lib/libquill_backend.so` — the only new runtime dependency of the
executable (see `ldd build/bin/chess`).

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
