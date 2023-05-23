cmake_minimum_required (VERSION 3.18)

if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
set(VCPKG_TARGET_TRIPLET x64-windows-static)

MACRO(GROUP_ALL_FILES SRCS)
# message("SRCS: ${SRCS}")
foreach(FILE ${SRCS}) 
    # message(STATUS "-----FILE: ${FILE}-----")
    # convert source file to absolute
    get_filename_component(ABSOLUTE_PATH "${FILE}" ABSOLUTE)
    # message(STATUS "ABSOLUTE_PATH: ${ABSOLUTE_PATH}")
    # Get the directory of the absolute source file
    get_filename_component(PARENT_DIR "${ABSOLUTE_PATH}" DIRECTORY)
    # message(STATUS "PARENT_DIR: ${PARENT_DIR}")
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # message(STATUS "GROUP: ${GROUP}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    # message(STATUS "GROUP: ${GROUP}")
    # Group into "Source Files" and "Header Files"
    source_group("${GROUP}" FILES "${FILE}")
  endforeach()
ENDMACRO(GROUP_ALL_FILES)

project(AirEngine)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")

add_definitions(-DNOMINMAX)

add_subdirectory (Runtime)
add_subdirectory (Demo)