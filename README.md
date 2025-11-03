<div align="center">

# Tremolo Audio Plugin 

Plugin project for the Official JUCE Audio Plugin Development Course

![JUCE audio plugin development course logo](docs/JUCE_Course_logo.svg)

</div>

## Requirements

* CMake 3.25 or higher (the one bundled with CLion 2025.1.1 or higher should work)
* C++ compiler and build system. Tested on
    - macOS Xcode 15.4 (Apple Clang 15.0.0.15000309), 16.4 (Apple Clang 17.0.0.17000013)
    - Windows: Visual Studio 2022 17.14.13 (MSVC 19.44.35215)
    - Ubuntu, Debian: gcc 12.2.0, 13.3.0 and make 4.3

## Usage

This is a template repository which means you can click "Use this template" on GitHub and create your own repo out of it.

The repository consists of two folders: *complete/* and *todo/*

*complete/* contains the completed tremolo plugin project. It compiles and runs out of the box.

*todo/* contains skeleton code of the same plugin. This is the code you will work throughout the course to gradually bring to the same state as the *complete/* folder. In its default state, it does not compile.

After cloning the repo you created from this template locally, you can proceed with the usual CMake workflow.

In the main repo directory execute

```bash
cd complete
cmake --preset default
cmake --build --preset default
```

The first run will take the most time because the dependencies (CPM and JUCE) need to be downloaded.

## License

We use the incredibly liberal ["Unlicense" license](LICENSE.md). You can basically do whatever you want with the code.

