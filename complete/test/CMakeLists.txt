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
  OPTIONS
    "BUILD_TESTS OFF"
)

# Creates the test console application.
add_executable(TremoloCoursePluginTest source/PluginProcessorTest.cpp source/JsonSerializerTest.cpp source/TremoloTest.cpp source/detail/StridedQueueTest.cpp)

# Configuration options that apply to the JUCE sources in the test target
target_compile_definitions(TremoloCoursePluginTest
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

# Thanks to the fact that we link against the gtest_main library, we don't have to write the main function ourselves.
target_link_libraries(
  TremoloCoursePluginTest
  PRIVATE
    ws::tremolo_plugin
    GTest::gtest_main
    wolfsound::wolfsound_dsp_utils
)

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
