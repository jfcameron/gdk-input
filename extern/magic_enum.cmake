# © 2019 Joseph Cameron - All Rights Reserved

file(COPY "${CMAKE_CURRENT_LIST_DIR}/magic_enum"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/magic_enum/")

add_library(magic_enum INTERFACE)

target_include_directories(magic_enum INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/magic_enum>")
