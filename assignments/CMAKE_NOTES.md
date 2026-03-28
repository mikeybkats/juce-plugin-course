## CMAKE `configure` command

**What `cmake` configure (e.g. `cmake --preset default` or `cmake -S src -B build`) does**

- **Reads** the top-level `CMakeLists.txt` from the **source directory** you give it (`-S` or current directory).
- **Creates or updates** the **build directory** (`-B` or preset `binaryDir`) and writes **`CMakeCache.txt`** (chosen compiler, paths, options, etc.).
- **Runs** all CMake logic in that `CMakeLists.txt` (and `add_subdirectory` / `include` chains): `project()`, `find_package()`, options, `add_library` / `add_executable` / `target_*`, custom commands, etc.
- **Resolves** generator-specific settings (Makefiles, Ninja, Xcode, Visual Studio, …) for that build tree.
- **Generates** the actual build files (e.g. `build.ninja`, `Makefile`, or IDE projects) in the build directory.
- **Does not compile** your sources; that happens on the **build** step (`cmake --build`).

**What it does _not_ do**

- Run the compiler/linker to produce final binaries (that’s **`cmake --build`**).
