cmake_minimum_required(VERSION 3.10)
project(obs_wsc)

if (MSVC)
        cmake_minimum_required(VERSION 3.16)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

add_subdirectory(deps)

set(wsc_SRC
    src/obs_wsc.c
    src/obs_wsc.h
    src/opaque.h
    src/util.h
    src/util.c
    src/misc_types.h
    src/send.c
    src/send.h
    src/messages/requests/general.c
    src/messages/requests/general.h
    src/messages/requests/outputs.c
    src/messages/requests/outputs.h
    src/messages/requests/profiles.c
    src/messages/requests/profiles.h
    src/messages/requests/recording.c
    src/messages/requests/recording.h
    src/messages/requests/replay.c
    src/messages/requests/replay.h
    src/external/buffer.c
    src/external/buffer.h
    src/external/mongoose.h
    src/external/mongoose.c
    src/external/bmem.h
    src/external/bmem.c
    src/external/dstr.h
    src/external/dstr.c
    src/external/darray.h
    src/external/utf8.c
    src/external/utf8.h
    src/data.c
    src/data.h
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

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

if (MSVC)
    set(wsc_PLATFORM_SRC
        src/external/platform-windows.c
        src/external/threading-windows.h)

elseif (UNIX)
    set(wsc_PLATFORM_SRC
        src/external/platform-nix.c
        src/external/threading-posix.h)
    add_definitions(-DUNIX=1)
    set(wsc_PLATFORM_DEPS
        pthread)
endif()

add_library("wsc" SHARED
    ${wsc_SRC}
    ${wsc_PLATFORM_SRC})

target_include_directories("wsc" PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${JANSSON_INCLUDE_DIRS})

target_link_libraries("wsc" ${JANSSON_IMPORT} ${wsc_PLATFORM_DEPS})

# Tests
add_executable("wsc_unit_tests"
    tests/wsc_unit_tests.c)
add_dependencies("wsc_unit_tests" "wsc")
target_link_libraries("wsc_unit_tests" "wsc")
target_include_directories("wsc_unit_tests" PUBLIC ${CHEAT_INCLUDE_DIRS})

add_executable("wsc_test"
    tests/wsc_test.c)
add_dependencies("wsc_test" "wsc")
target_link_libraries("wsc_test" "wsc")
