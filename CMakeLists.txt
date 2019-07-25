# © 2019 Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/jfc-cmake/jfclib.cmake")

option(JFC_BUILD_DEMOS "Build the app" ON)
option(JFC_BUILD_DOCS  "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" ON)

add_subdirectory(thirdparty)

jfc_project(library
    NAME "gdkinput"
    VERSION 0.0
    DESCRIPTION "game development kit human input device library"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${glfw_INCLUDE_DIR}

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include

    LIBRARIES
        ${glfw_LIBRARIES}

    DEPENDENCIES
        "glfw"

    SOURCE_LIST 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/controls.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gamepads_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard_glfw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mouse_glfw.cpp
)

#[[if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()]]

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if (JFC_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

