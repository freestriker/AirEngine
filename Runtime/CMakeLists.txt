cmake_minimum_required (VERSION 3.18)

project (Runtime CXX)

file(GLOB_RECURSE ALL_HEADER_FILES "${PROJECT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE ALL_SOURCE_FILES "${PROJECT_SOURCE_DIR}/*.cpp")
set(ALL_FILES ${ALL_HEADER_FILES} ${ALL_SOURCE_FILES})

GROUP_ALL_FILES("${ALL_FILES}")

# add_definitions(-DDLL_EXPORTS)

add_library(Runtime STATIC)

target_sources(Runtime PUBLIC ${ALL_FILES})

find_package(assimp CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC assimp::assimp)

find_package(BDWgc CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC BDWgc::gc BDWgc::cord BDWgc::gccpp BDWgc::gctba)

target_include_directories(Runtime PUBLIC ${Boost_INCLUDE_DIRS})
find_package(Boost COMPONENTS fiber REQUIRED)
target_link_libraries(Runtime PUBLIC Boost::fiber)

find_package(cityhash CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC cityhash)

find_package(glm CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC glm::glm)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC nlohmann_json::nlohmann_json)

find_package(OpenCV REQUIRED)
target_link_libraries(Runtime PUBLIC ${OpenCV_LIBS})

set_target_properties(Runtime PROPERTIES AUTOMOC ON)
find_package(Qt6Core CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC Qt::Core Qt6::Core Qt::CorePrivate Qt6::CorePrivate)

find_package(Qt6Gui CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC Qt::Gui Qt6::Gui Qt::GuiPrivate Qt6::GuiPrivate)

find_package(Qt6Widgets CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC Qt::Widgets Qt6::Widgets Qt::WidgetsPrivate Qt6::WidgetsPrivate)

find_package(rttr CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC RTTR::Core_Lib)

find_package(unofficial-spirv-reflect CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC unofficial::spirv-reflect::spirv-reflect)

find_package(vk-bootstrap CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC vk-bootstrap::vk-bootstrap)

find_package(Vulkan REQUIRED)
target_link_libraries(Runtime PUBLIC Vulkan::Vulkan)

find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
target_link_libraries(Runtime PUBLIC unofficial::vulkan-memory-allocator::vulkan-memory-allocator)

set_property(TARGET Runtime PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")