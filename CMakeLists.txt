# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET gdkinput)
    return()
endif()

project(gdk-input
    VERSION 1.0
    DESCRIPTION "the gdk input interface: keyboard, mouse, gamepads and text, controls over them, and a null implementation"
    LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

option(GDKINPUT_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GDKINPUT_BUILD_DOCS "Build documentation" OFF)
option(GDKINPUT_EXT_LUA "Build the ext/ lua bindings codec, which depends on jfc-lua" OFF)

if (GDKINPUT_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        PROJECT_LOGO     ${CMAKE_CURRENT_LIST_DIR}/docs/icon.png

        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
            impl/null/include/
    )

    if (NOT GDKINPUT_BUILD_TESTS)
        return()
    endif()
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

add_subdirectory(extern)

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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(gdkinput STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/binding.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/capture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/controls.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/enum_ostreams.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/text_input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_context.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/src/null_gamepad.cpp")

add_library(gdk::input ALIAS gdkinput)

set_target_properties(gdkinput PROPERTIES EXPORT_NAME input PREFIX "lib")

target_include_directories(gdkinput
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_compile_features(gdkinput PUBLIC cxx_std_20)

target_link_libraries(gdkinput PRIVATE $<BUILD_INTERFACE:magic_enum>)

add_library(gdkinput_implementation INTERFACE)

target_include_directories(gdkinput_implementation INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

target_link_libraries(gdkinput_implementation INTERFACE gdkinput magic_enum)

add_library(gdkinput_conformance INTERFACE)

target_sources(gdkinput_conformance INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/controls_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/gamepad_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/keyboard_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/mouse_conformance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance/text_conformance.cpp")

target_include_directories(gdkinput_conformance INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/test/conformance")

target_link_libraries(gdkinput_conformance INTERFACE gdkinput)

if (GDKINPUT_EXT_LUA)
    add_library(gdkinput_ext_lua STATIC "${CMAKE_CURRENT_SOURCE_DIR}/src/ext/lua_bindings.cpp")

    add_library(gdk::input_ext_lua ALIAS gdkinput_ext_lua)

    target_compile_features(gdkinput_ext_lua PUBLIC cxx_std_20)

    target_link_libraries(gdkinput_ext_lua PRIVATE magic_enum)

    target_link_libraries(gdkinput_ext_lua PUBLIC gdkinput jfclua)
endif()

install(TARGETS gdkinput
    EXPORT gdk-input-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/impl/null/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    PATTERN "ext" EXCLUDE)

install(EXPORT gdk-input-targets
    FILE gdk-input-targets.cmake
    NAMESPACE gdk::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-input")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/gdk-input-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

file(WRITE "${PROJECT_BINARY_DIR}/gdk-input-config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/gdk-input-targets.cmake\")
")

install(FILES
    "${PROJECT_BINARY_DIR}/gdk-input-config.cmake"
    "${PROJECT_BINARY_DIR}/gdk-input-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/gdk-input")

if (GDKINPUT_BUILD_TESTS)
    add_subdirectory(test)
endif()
