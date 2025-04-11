# Tremolo

![Cmake workflow success badge](https://github.com/JanWilczek/audio-plugin-template/actions/workflows/cmake.yml/badge.svg)

## Usage

This is a template repository which means you can right click "Use this template" on GitHub and create your own repo out of it.

After cloning it locally, you can proceed with the usual CMake workflow.

In the main repo directory execute

```bash
 # uses Ninja; alternatives: release | vs | Xcode
 # BUILD_TESTS flag will donwload googletest and allow unit test execution
cmake --preset default -DBUILD_TESTS=ON
cmake --build --preset default

# to run unit tests (BUILD_TESTS=ON)
ctest --preset default
```

The first run will take the most time because the dependencies (CPM, JUCE, and optionally googletest) need to be downloaded.

Existing presets are `default`, `release`, `vs`, and `Xcode`.

To run clang-format on every commit, in the main directory execute

```bash
pre-commit install
```

(for this you may need to install `pre-commit` with `pip`: `pip install pre-commit`).

