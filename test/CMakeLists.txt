cmake_minimum_required(VERSION 3.22)

project(TremoloCoursePluginTest)

# Signals to CMake that we want to run tests from this directory.
enable_testing()

# Adds googletest.
cpmaddpackage(
  NAME
    GOOGLETEST
  GITHUB_REPOSITORY
    google/googletest
  VERSION
    1.16.0
  SOURCE_DIR
    ${LIB_DIR}/googletest
  OPTIONS
    "INSTALL_GTEST OFF"
    "gtest_force_shared_crt ON"
)

cpmaddpackage(
  NAME
    WOLFSOUND_DSP_UTILS
  GITHUB_REPOSITORY
    JanWilczek/wolfsound-dsp-utils
  VERSION
    0.3.0
  SOURCE_DIR
    ${LIB_DIR}/wolfsound-dsp-utils
)

# Creates the test console application.
set(SOURCE_FILES source/PluginProcessorTest.cpp source/JsonSerializerTest.cpp source/TremoloTest.cpp)
add_executable(TremoloCoursePluginTest ${SOURCE_FILES})

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(
  TremoloCoursePluginTest
  PRIVATE
    ws::TremoloCoursePlugin
    GTest::gtest_main
    wolfsound::wolfsound_dsp_utils
)

# Enables strict C++ warnings and treats warnings as errors.
# This needs to be set up only for your projects, not 3rd party
set_source_files_properties(${SOURCE_FILES} PROPERTIES COMPILE_OPTIONS "${PROJECT_WARNINGS_CXX}")

# Adds googletest-specific CMake commands at our disposal.
include(GoogleTest)

# Add all tests defined with googletest to the CMake metadata
# so that these tests are run upon a call to ctest in the test
# projects' binary directory.
if(CMAKE_GENERATOR STREQUAL Xcode)
  # On macOS arm64, all binaries have to be signed before running.
  # In local development, the linker adds an ad-hoc placeholder signature.
  # In Xcode however, the ad-hoc signature is delayed until after the “Run Script” build phase,
  # so the POST_BUILD command added by gtest_discover_tests cannot run.
  # Thus, we need to delay test discovery until run time.
  # Source: https://discourse.cmake.org/t/googletest-crash-when-using-cmake-xcode-arm64/5766/8
  gtest_discover_tests(TremoloCoursePluginTest DISCOVERY_MODE PRE_TEST)
else()
  gtest_discover_tests(TremoloCoursePluginTest)
endif()
