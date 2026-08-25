# © Joseph Cameron - All Rights Reserved

jfc_project(executable
    NAME "gdkinput-simple-demo"
    VERSION 1.0
    DESCRIPTION "demo gdk-input app"
    C++_STANDARD 20
    C_STANDARD 90

    SOURCE_LIST
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    
    PRIVATE_INCLUDE_DIRECTORIES
        "${gdkinput_INCLUDE_DIRECTORIES}"
        "${glfw_INCLUDE_DIR}"

    LIBRARIES
        "${gdkinput_LIBRARIES}"

    DEPENDENCIES
        "gdkinput"
)

