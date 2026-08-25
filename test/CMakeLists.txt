# © Joseph Cameron - All Rights Reserved

if (GDK_INPUT_EXT_LUA)
    set(ext_test_sources "${CMAKE_CURRENT_LIST_DIR}/ext_lua_bindings_test.cpp")
    set(ext_test_includes ${gdkinput_ext_lua_INCLUDE_DIRECTORIES})
    set(ext_test_libraries ${gdkinput_ext_lua_LIBRARIES})
endif()

jfc_add_tests(
    C++_STANDARD 20
    C_STANDARD 90

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/null_backend_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/controls_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/capture_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/context_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/gamepad_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/keyboard_test.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mouse_test.cpp"
        ${ext_test_sources}

    INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}"
        "${gdkinput_INCLUDE_DIRECTORIES}"
        "${magic_enum_INCLUDE_DIR}"
        ${ext_test_includes}

    LIBRARIES
        "${gdkinput_LIBRARIES}"
        "${glfw_LIBRARIES}"
        ${ext_test_libraries})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)
