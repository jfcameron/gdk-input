# © Joseph Cameron - MIT License

if (NOT TARGET magic_enum)
    include("${CMAKE_CURRENT_LIST_DIR}/magic_enum.cmake")
endif()

if (GDKINPUT_EXT_LUA AND NOT TARGET jfclua)
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/jfc-lua/CMakeLists.txt")
        message(FATAL_ERROR "GDKINPUT_EXT_LUA needs jfc-lua: run git submodule update --init --recursive")
    endif()

    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/jfc-lua")
endif()
