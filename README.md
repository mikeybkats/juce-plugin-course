# Tremolo Audio Plugin 

Plugin project for the Official JUCE Audio Plugin Development Course


## Usage

This is a template repository which means you can click "Use this template" on GitHub and create your own repo out of it.

After cloning it locally, you can proceed with the usual CMake workflow.

In the main repo directory execute

```bash
# 'default' preset uses Ninja; alternatives: 'release' | 'vs' | 'Xcode'
# BUILD_TESTS flag will download googletest and allow unit test execution
# WARNINGS_AS_ERRORS flag will disallow code compilation on possible mistakes
cmake --preset default -DBUILD_TESTS=ON -DWARNINGS_AS_ERRORS=ON
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
