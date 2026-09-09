# © 2018 Joseph Cameron - All Rights Reserved

if (TARGET glfw)
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
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Windows")
        #[[find_package(OpenGL REQUIRED)

        #project("GLEW") # Its weird to hide glew here but it works...

        add_library(${PROJECT_NAME} STATIC
            ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/src/glew.c)

        target_include_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include)

        set_target_properties(${PROJECT_NAME} PROPERTIES
            RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")

        set(GLEW_LIBRARIES ${PROJECT_BINARY_DIR}/libGLEW.a)
        set(GLEW_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0/include)]]

        if(CMAKE_SYSTEM_NAME MATCHES "Linux")
            find_package(X11 REQUIRED) # Mir? Wayland?
            find_package(Threads REQUIRED)
        endif()
    endif()
else()
    message(FATAL_ERROR "${PROJECT_NAME}.cmake has not been configured to handle platform \"${CMAKE_SYSTEM_NAME}\".")
endif()

target_include_directories(glfw INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/glfw/include>"

    # Graphics interface
    ${OPENGL_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIR}

    # Linux or Windows
    ${GLEW_INCLUDE_DIR}

    # Linux
    ${X11_INCLUDE_DIR})

set_property(TARGET glfw APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    # Linux or Windows
    ${GLEW_LIBRARIES}
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

