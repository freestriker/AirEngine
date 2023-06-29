cmake_minimum_required (VERSION 3.18)

project (Demo CXX)

file(GLOB_RECURSE ALL_HEADER_FILES ${PROJECT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
set(ALL_FILES ${ALL_HEADER_FILES} ${ALL_SOURCE_FILES})

GROUP_ALL_FILES("${ALL_FILES}")

include_directories(${PROJECT_SOURCE_DIR}/../Runtime)

add_executable(Demo)

target_sources(Demo PRIVATE ${ALL_FILES})

set_target_properties(Demo PROPERTIES AUTOMOC ON)
target_link_libraries(Demo PRIVATE Runtime)

set_property(TARGET Demo PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")