# © Joseph Cameron - MIT License

set(_gdk_input_ext_tests)
set(_gdk_input_ext_libraries)

if (GDKINPUT_EXT_LUA)
    set(_gdk_input_ext_tests "${CMAKE_CURRENT_LIST_DIR}/ext_lua_bindings_test.cpp")
    set(_gdk_input_ext_libraries gdkinput_ext_lua)
endif()

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/capture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/controls_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/enum_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_backend_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/null_conformance.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/text_test.cpp"
        ${_gdk_input_ext_tests}

    LIBRARIES
        gdkinput_implementation
        gdkinput_conformance
        ${_gdk_input_ext_libraries})

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

if (NOT CMAKE_CROSSCOMPILING)
    add_test(NAME gdk-input_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DTOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
