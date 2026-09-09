# © Joseph Cameron - All Rights Reserved

project(gdkinput-simple-demo)

add_executable(gdkinput-simple-demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(gdkinput-simple-demo PRIVATE
    "${glfw_INCLUDE_DIR}")

target_link_libraries(gdkinput-simple-demo PRIVATE gdkinput)

target_compile_features(gdkinput-simple-demo PRIVATE cxx_std_20)

