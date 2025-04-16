cmake_minimum_required(VERSION 3.22)

project(TremoloCoursePlugin VERSION 0.1.0)

# Adds a plugin target.
juce_add_plugin(
  TremoloCoursePlugin
  PRODUCT_NAME
    "Tremolo"
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
    TRUE
)
# Adding an alias for a target is a good CMake practice: it forces
# CMake to fail project generation when the aliased target doesn't exist.
add_library(ws::TremoloCoursePlugin ALIAS TremoloCoursePlugin)

# Sets the source files of the plugin project.
set(SOURCE_FILES
    source/CustomLookAndFeel.cpp
    source/PluginEditor.cpp
    source/PluginProcessor.cpp
    source/Parameters.cpp
    source/JsonSerializer.cpp
)
juce_add_module(tremolo_plugin_core ALIAS_NAMESPACE ws)
target_sources(TremoloCoursePlugin PRIVATE ${SOURCE_FILES})

# Sets the include directories of the plugin project.
target_include_directories(TremoloCoursePlugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Links to all necessary dependencies.
target_link_libraries_system(
  TremoloCoursePlugin
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
target_link_libraries(
  TremoloCoursePlugin
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# These definitions are recommended by JUCE.
target_compile_definitions(TremoloCoursePlugin
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Enables strict C++ warnings and treats warnings as errors.
# We set it only for our source files to avoid compilation errors for JUCE sources.
set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

# In Visual Studio this command provides a nice grouping of source files in "filters".
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/..)
