find_package(Jansson 2.5 QUIET)

if(NOT JANSSON_FOUND)
	message(STATUS "Jansson >=2.5 not found, building bundled version")

	add_subdirectory(jansson)

        set(JANSSON_IMPORT "jansson" CACHE INTERNAL "Internal var")
        set(JANSSON_INCLUDE_DIRS "" CACHE INTERNAL "Internal var")
else()
	message(STATUS "Using system Jansson library")

        set(JANSSON_IMPORT "${JANSSON_LIBRARIES}" CACHE INTERNAL "Internal var")
        set(JANSSON_INCLUDE_DIRS "${JANSSON_INCLUDE_DIRS}" CACHE INTERNAL "Internal var")
endif()

set(CHEAT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cheat" CACHE INTERNAL "Internal var")
