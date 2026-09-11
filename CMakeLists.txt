# © Joseph Cameron - All Rights Reserved

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

project(gdkinput
    VERSION 0.9
    DESCRIPTION "game development kit human input device library")

option(JFC_BUILD_DEMO "Build demos" ON)
option(JFC_BUILD_DOCS  "Build documentation" ON)
option(JFC_BUILD_TESTS "Build unit tests" OFF)

option(GDK_INPUT_EXT_LUA "Build the ext/ lua bindings codec, which depends on jfc-lua" OFF)

set(GDK_INPUT_JFC_LUA_PATH "" CACHE PATH "a jfc-lua checkout to use instead of the submodule")

set(GDK_INPUT_IMPLEMENTATION_GLFW ON)
set(GDK_INPUT_IMPLEMENTATION_NULL ON)

add_subdirectory(extern)

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

if (GDK_INPUT_IMPLEMENTATION_NULL)
    set(implementation_source_list "${implementation_source_list}"
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_gamepad.cpp
    )

    set(implementation_header_paths "${implementation_header_paths}"
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include)
endif()

file(GLOB_RECURSE _gdk_input_own_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/impl/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/gdk/input/*.h")

foreach(_source ${_gdk_input_own_sources})
    file(READ "${_source}" _contents)

    if (_contents MATCHES "#include[ \t]*<gdk/input\\.h>")
        message(FATAL_ERROR
            "${_source} includes the umbrella <gdk/input.h>. It is for consumers; include the "
            "individual headers this file actually needs.")
    endif()
endforeach()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

add_library(gdkinput STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/binding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/capture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controls.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/enum_ostreams.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
    ${implementation_source_list})

add_library(gdk::input ALIAS gdkinput)

set_target_properties(gdkinput PROPERTIES EXPORT_NAME input)

set_target_properties(gdkinput PROPERTIES PREFIX "lib")

jfc_generate_build_info()

target_include_directories(gdkinput
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        ${implementation_header_paths}
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        "${PROJECT_BINARY_DIR}/generated_include/"
        ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    )

target_compile_features(gdkinput PUBLIC cxx_std_20)

target_link_libraries(gdkinput PUBLIC
    glfw)

target_link_libraries(gdkinput PRIVATE magic_enum)

if (GDK_INPUT_EXT_LUA)
    if (NOT TARGET jfclua)
        set(_gdk_input_jfc_lua "${GDK_INPUT_JFC_LUA_PATH}")

        if (NOT _gdk_input_jfc_lua)
            set(_gdk_input_jfc_lua "${CMAKE_CURRENT_SOURCE_DIR}/extern/jfc-lua")
        endif()

        if (NOT EXISTS "${_gdk_input_jfc_lua}/CMakeLists.txt")
            message(FATAL_ERROR
                "GDK_INPUT_EXT_LUA needs jfc-lua, and ${_gdk_input_jfc_lua} has no CMakeLists.txt. "
                "Run: git submodule update --init --recursive")
        endif()

        set(_gdk_input_outer_tests "${JFC_BUILD_TESTS}")
        set(_gdk_input_outer_docs "${JFC_BUILD_DOCS}")
        set(_gdk_input_outer_demo "${JFC_BUILD_DEMO}")
        set(_gdk_input_outer_bench "${JFC_BUILD_BENCH}")

        set(JFC_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(JFC_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(JFC_BUILD_DEMO OFF CACHE BOOL "" FORCE)
        set(JFC_BUILD_BENCH OFF CACHE BOOL "" FORCE)

        add_subdirectory("${_gdk_input_jfc_lua}" "${CMAKE_CURRENT_BINARY_DIR}/jfc-lua")

        set(JFC_BUILD_TESTS "${_gdk_input_outer_tests}" CACHE BOOL "" FORCE)
        set(JFC_BUILD_DOCS "${_gdk_input_outer_docs}" CACHE BOOL "" FORCE)
        set(JFC_BUILD_DEMO "${_gdk_input_outer_demo}" CACHE BOOL "" FORCE)
        set(JFC_BUILD_BENCH "${_gdk_input_outer_bench}" CACHE BOOL "" FORCE)
    endif()

    add_library(gdkinput_ext_lua STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/lua_bindings.cpp")

    target_compile_features(gdkinput_ext_lua PUBLIC cxx_std_20)

    target_include_directories(gdkinput_ext_lua
        PUBLIC
            "${CMAKE_CURRENT_SOURCE_DIR}/include")

    target_link_libraries(gdkinput_ext_lua PRIVATE magic_enum)

    target_link_libraries(gdkinput_ext_lua PUBLIC gdkinput jfclua)

    set(gdkinput_ext_lua_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_SOURCE_DIR}/include")
    set(gdkinput_ext_lua_LIBRARIES gdkinput_ext_lua)
endif()

if (JFC_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_DOCS)
    add_subdirectory(docs)
endif()

if (JFC_BUILD_DEMO)
    add_subdirectory(demos)
endif()

