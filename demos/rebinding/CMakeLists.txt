# © Joseph Cameron - All Rights Reserved

if (TARGET gdkinput_ext_lua)
    set(rebinding_demo_includes "${gdkinput_ext_lua_INCLUDE_DIRECTORIES}")
    set(rebinding_demo_libraries "${gdkinput_ext_lua_LIBRARIES}")
    set(rebinding_demo_has_lua 1)
else()
    set(rebinding_demo_has_lua 0)
endif()

jfc_project(executable
    NAME "gdkinput-rebinding-demo"
    VERSION 1.0
    DESCRIPTION "bindings and interactive rebinding"
    C++_STANDARD 20
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkinput_INCLUDE_DIRECTORIES}"
        "${glfw_INCLUDE_DIR}"
        ${rebinding_demo_includes}

    LIBRARIES
        "${gdkinput_LIBRARIES}"
        ${rebinding_demo_libraries}

    DEPENDENCIES
        "gdkinput"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE "GDK_INPUT_DEMO_HAS_LUA=${rebinding_demo_has_lua}")
