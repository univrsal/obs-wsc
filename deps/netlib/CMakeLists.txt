project (netlib C)

if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(netlib_PLATFORM_DEPS
            ws2_32
            iphlpapi)
        add_definitions(-DWIN64=1)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(netlib_PLATFORM_DEPS
            wsock32
            iphlpapi)
        add_definitions(-DWIN32=1)
    endif()

    add_definitions(-DWINDOWS=1)
elseif(UNIX)
    add_definitions(-DUNIX=1)
endif()

set(NETLIB_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netlib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netlib_select.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netlib_tcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netlib_udp.c)

set(NETLIB_HDR_PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/src/platform.h)

set(NETLIB_HDR_PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/src/util.h
   ${CMAKE_CURRENT_SOURCE_DIR}/src/netlib.h)

source_group("Library Sources" FILES ${NETLIB_SRC})
source_group("Library Private Headers" FILES ${NETLIB_HDR_PRIVATE})
source_group("Library Public Headers" FILES ${NETLIB_HDR_PUBLIC})

add_library(netlib
    ${NETLIB_SRC}
    ${NETLIB_HDR_PRIVATE}
    ${NETLIB_HDR_PUBLIC})

set(NETLIB_LIBRARIES netlib CACHE STRING "Netlib libraries")
set(NETLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Netlib includes")

target_include_directories(netlib
        PUBLIC src "${CMAKE_CURRENT_BINARY_DIR}/src")
