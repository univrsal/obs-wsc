project(obs_wsc)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(deps)

set(obs_wsc_SRC
    src/obs_wsc.c
    src/obs_wsc.h
    src/opaque.h
    src/util.h
    src/util.c
    src/misc_types.h
    src/send.c
    src/send.h
    src/external/mongoose.h
    src/external/mongoose.c
    src/external/bmem.h
    src/external/bmem.c
    src/external/dstr.h
    src/external/dstr.c
    src/external/utf8.c
    src/external/utf8.h
    src/external/lexer.c
    src/external/lexer.h
    src/external/base64.h
    src/external/base64.c
    src/external/sha256.h
    src/external/sha256.c
    src/external/bmem.h
    src/external/c99defs.h
    src/external/base.c
    src/external/base.h
    src/external/platform.h
    src/external/platform.c
    src/external/threading.h)

if (MSVC)
    set(obs_wsc_PLATFORM_SRC
        src/external/platform-windows.c
        src/external/threading-windows.h)
elseif (UNIX)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")
    set(obs_wsc_PLATFORM_SRC
        src/external/platform-nix.c
        src/external/threading-posix.h)
    add_definitions(-DUNIX=1)
    set(obs_wsc_PLATFORM_DEPS
        pthread)
endif()

add_library("obs_wsc" SHARED
    ${obs_wsc_SRC}
    ${obs_wsc_PLATFORM_SRC})

target_include_directories("obs_wsc" PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${JANSSON_INCLUDE_DIRS})

target_link_libraries("obs_wsc" ${JANSSON_IMPORT} ${obs_wsc_PLATFORM_DEPS})

# Tests
add_executable("wsc_unit_tests"
    tests/wsc_unit_tests.c)
add_dependencies("wsc_unit_tests" "obs_wsc")
target_link_libraries("wsc_unit_tests" "obs_wsc")
target_include_directories("wsc_unit_tests" PUBLIC ${CHEAT_INCLUDE_DIRS})

add_executable("wsc_test"
    tests/wsc_test.c)
add_dependencies("wsc_test" "obs_wsc")
target_link_libraries("wsc_test" "obs_wsc")
