# © 2018 Joseph Cameron - All Rights Reserved

if (TARGET glfw)
    return()
endif()

if (EMSCRIPTEN)
    add_library(glfw INTERFACE)

    target_include_directories(glfw INTERFACE "$<BUILD_INTERFACE:${EMSCRIPTEN_SYSROOT}/include>")


    target_link_options(glfw INTERFACE "-sUSE_GLFW=3")

    set(glfw_INCLUDE_DIR "${EMSCRIPTEN_SYSROOT}/include")

    return()
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

add_subdirectory(glfw)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin" OR CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set (OpenGL_GL_PREFERENCE "GLVND") 
    endif()

    find_package(OpenGL REQUIRED) 

    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        FIND_LIBRARY(COCOA_LIBRARY Cocoa)
        FIND_LIBRARY(CORE_VIDEO CoreVideo)
        FIND_LIBRARY(IO_KIT IOKit)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        find_package(X11 REQUIRED) # Mir? Wayland?
        find_package(Threads REQUIRED)
    endif()
else()
    message(FATAL_ERROR "${PROJECT_NAME}.cmake has not been configured to handle platform \"${CMAKE_SYSTEM_NAME}\".")
endif()

target_include_directories(glfw INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glfw/include>"

    # Graphics interface
    ${OPENGL_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIR}

    # Linux
    ${X11_INCLUDE_DIR})

set_property(TARGET glfw APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    ${CMAKE_DL_LIBS}

    # Graphics interface
    ${OPENGL_LIBRARIES}
    ${Vulkan_LIBRARIES}

    # Macos
    ${COCOA_LIBRARY}
    ${CORE_VIDEO}
    ${IO_KIT}

    # Linux specific
    ${X11_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

