project(obs_wsc)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(deps)

set(obs_wsc_SRC
    src/obs_wsc.c
    src/obs_wsc.h
    src/opaque.h
    src/util.h
    src/misc_types)

if (UNIX)
    add_definitions(-DUNIX=1)
endif()

add_library("obs-wsc" SHARED ${obs_wsc_SRC})
target_include_directories("obs-wsc" PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${JANSSON_INCLUDE_DIRS}
    ${NETLIB_INCLUDE_DIRS})
message(STATUS ${NETLIB_INCLUDE_DIRS})
target_link_libraries("obs-wsc" ${NETLIB_IMPORT} ${JANSSON_IMPORT})

# Test
add_executable("wsc_tests" "tests/wsc_tests.c")
add_dependencies("wsc_tests" "obs-wsc")
target_link_libraries("wsc_tests" "obs-wsc")
target_include_directories("wsc_tests" PUBLIC ${CHEAT_INCLUDE_DIRS})
