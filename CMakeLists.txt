# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(null)

option(JFC_BUILD_DEMO "Build demos" ON)
option(JFC_BUILD_DOCS  "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

set(GDK_INPUT_IMPLEMENTATION_GLFW ON)

add_subdirectory(thirdparty)

if (GDK_INPUT_IMPLEMENTATION_GLFW) 
    set(implementation_source_list "${implementation_source_list}"
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/glfw/src/glfw_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/glfw/src/glfw_gamepad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/glfw/src/glfw_keyboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/glfw/src/glfw_mouse.cpp
    )

    set(implementation_header_paths "${implementation_header_paths}"
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/glfw/include)
endif()

jfc_project(library
    NAME "gdkinput"
    VERSION 0.9
    DESCRIPTION "game development kit human input device library"
    C++_STANDARD 17
    C_STANDARD 90
    TYPE STATIC

    DEPENDENCIES
        "glfw"

    LIBRARIES
        ${glfw_LIBRARIES}

    PUBLIC_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${implementation_header_paths}
        ${glfw_INCLUDE_DIR}

    PRIVATE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
        ${nlohmann_INCLUDE_DIR}
        ${magic_enum_INCLUDE_DIR}
        ${jsoncons_INCLUDE_DIR}

    SOURCE_LIST 
		${CMAKE_CURRENT_SOURCE_DIR}/src/enum_ostreams.cpp
        ${implementation_source_list}
)

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if (JFC_BUILD_DEMO)
    add_subdirectory(demos)
endif()

