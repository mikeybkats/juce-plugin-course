cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)

project(JucePluginDevelopmentCourseExamples VERSION 1.0.0)

if(PROJECT_IS_TOP_LEVEL)
  if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
  endif()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)
endif()

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/cpm.cmake)

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

function(course_add_example_app target)
  juce_add_gui_app(${target}
    PRODUCT_NAME "${target}Example"
  )
  target_compile_definitions(${target}
      PRIVATE
          JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
          JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
          JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${target},JUCE_PRODUCT_NAME>"
          JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${target},JUCE_VERSION>")

  target_sources(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../Main.cpp)

  target_link_libraries(${target} PRIVATE juce::juce_gui_basics)

  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

add_subdirectory(Coordinates)
add_subdirectory(UserEventHandlers)
