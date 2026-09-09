# © Joseph Cameron - All Rights Reserved

if (TARGET gdkinput_ext_lua)
    set(rebinding_demo_includes "${gdkinput_ext_lua_INCLUDE_DIRECTORIES}")
    set(rebinding_demo_libraries "${gdkinput_ext_lua_LIBRARIES}")
    set(rebinding_demo_has_lua 1)
else()
    set(rebinding_demo_has_lua 0)
endif()

project(gdkinput-rebinding-demo)

add_executable(gdkinput-rebinding-demo
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_include_directories(gdkinput-rebinding-demo PRIVATE
    "${glfw_INCLUDE_DIR}"
    ${rebinding_demo_includes})

target_link_libraries(gdkinput-rebinding-demo PRIVATE gdkinput ${rebinding_demo_libraries})

target_compile_features(gdkinput-rebinding-demo PRIVATE cxx_std_20)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE "GDK_INPUT_DEMO_HAS_LUA=${rebinding_demo_has_lua}")
