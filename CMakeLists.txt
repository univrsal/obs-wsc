project(obs_wsc)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(deps)

set(obs_wsc_SRC
    src/obs_wsc.c
    src/obs_wsc.h
    src/opaque.h
    src/util.h
    src/misc_types.h
    src/send.c
    src/send.h
    src/external/bmem.c
    src/external/bmem.h
    src/external/c99defs.h
    src/external/base.c
    src/external/base.h
    src/external/platform.h
    src/external/threading.h)

if (MSVC)
    set(obs_wsc_PLATFORM_SRC
        src/external/platform-windows.c
        src/external/threading-windows.h)
elseif (UNIX)
    set(obs_wsc_PLATFORM_SRC
        src/external/platform-nix.c
        src/external/threading-posix.h)
    add_definitions(-DUNIX=1)
endif()

add_library("obs_wsc" SHARED
    ${obs_wsc_SRC}
    ${obs_wsc_PLATFORM_SRC})
target_include_directories("obs_wsc" PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${JANSSON_INCLUDE_DIRS}
    ${NETLIB_INCLUDE_DIRS})

target_link_libraries("obs_wsc" ${NETLIB_IMPORT} ${JANSSON_IMPORT})

# Test
add_executable("wsc_tests"
    tests/wsc_tests.c
    tests/wsc_tests.h)
add_dependencies("wsc_tests" "obs_wsc")
target_link_libraries("wsc_tests" "obs_wsc")
target_include_directories("wsc_tests" PUBLIC ${CHEAT_INCLUDE_DIRS})
