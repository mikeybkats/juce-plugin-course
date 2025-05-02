# Mandatory line, sets the minimum version of CMake that should be used with this repository.
# 3.22 is the minimum CMake version required by JUCE.
# To verify your version run
# $ cmake --version
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)

# Sets a few variables, like PROJECT_NAME.
project(JucePluginDevelopmentCourseTremoloPlugin)

# Set the C++ standard only if this project is standalone;
# otherwise, let clients decide.
if(PROJECT_IS_TOP_LEVEL)
  # Always use the newest C++ standard on green-field projects if possible.
  # C++ 20 is a good baseline.
  set(CMAKE_CXX_STANDARD 20)

  # Link to C++ runtime statically
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# I like to download the dependencies to the same folder as the project.
# If you want to install them system-wide, set CPM_SOURCE_CACHE with a path to the dependencies
# either as an environment variable or pass it with -DCPM_SOURCE_CACHE=<path> when invoking cmake.
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
cpmaddpackage(
  NAME
    JUCE
  GIT_TAG
    8.0.7
  GITHUB_REPOSITORY
    juce-framework/JUCE
  SOURCE_DIR
    ${LIB_DIR}/juce
)

# Adds compiler warning utilities & other.
include(cmake/CompilerWarnings.cmake)
include(cmake/Util.cmake)

# Adds a plugin target.
set(TREMOLO_PLUGIN_NAME "Tremolo")
juce_add_plugin(
  TremoloCoursePlugin
  PRODUCT_NAME
    ${TREMOLO_PLUGIN_NAME}
  VERSION
    0.0.0 # 0 forces AU hosts to always refresh
  COMPANY_NAME
    "WolfSound"
  PLUGIN_MANUFACTURER_CODE
    Wfsd
  PLUGIN_CODE
    Trcp
  FORMATS
    Standalone
    AU
    VST3
  IS_SYNTH
    FALSE
  NEEDS_MIDI_INPUT
    FALSE
  NEEDS_MIDI_OUTPUT
    FALSE
  VST3_AUTO_MANIFEST
    FALSE
  COPY_PLUGIN_AFTER_BUILD
    FALSE
)

# Configure the JUCE sources included in the plugin target.
target_compile_definitions(TremoloCoursePlugin
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_add_module(tremolo_plugin ALIAS_NAMESPACE ws)

# Additional definitions required by all users of this module.
target_compile_definitions(tremolo_plugin
  INTERFACE
    TREMOLO_PLUGIN_NAME="${TREMOLO_PLUGIN_NAME}"
)

# Enables strict C++ warnings.
target_compile_options(tremolo_plugin INTERFACE "${PROJECT_WARNINGS_CXX}")

target_link_libraries(
  TremoloCoursePlugin
  PRIVATE
    ws::tremolo_plugin
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)

option(BUILD_TESTS "When on, will download the GoogleTest framework and add the automated tests project" OFF)
if(BUILD_TESTS)
  # This command allows running tests from the "build" folder (the one where CMake generates the project to).
  enable_testing()

  # Adds all the targets configured in the "test" folder.
  add_subdirectory(test)
endif()
