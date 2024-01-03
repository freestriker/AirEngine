cmake_minimum_required (VERSION 3.25)

function(FIND_AND_GROUP_RUNTIME_SOURCE_FILES RESULT_RUNTIME_SOURCE_FILE_PATHS)
    message(STATUS "-----FIND_AND_GROUP_RUNTIME_SOURCE_FILES: START-----")
    FIND_SOURCE_FILE_PATHS("${RUNTIME_DIRECTORY_PATH}" RUNTIME_SOURCE_FILE_PATHS)
    message(STATUS "-----RUNTIME_SOURCE_FILE_PATHS: ${RUNTIME_SOURCE_FILE_PATHS}-----")
    message(STATUS "-----AIR_ENGINE_DIRECTORY_PATH: ${AIR_ENGINE_DIRECTORY_PATH}-----")
    GROUP_FILES_BY_DIRECTORY(${AIR_ENGINE_DIRECTORY_PATH} "${RUNTIME_SOURCE_FILE_PATHS}")
    set(${RESULT_RUNTIME_SOURCE_FILE_PATHS} ${RUNTIME_SOURCE_FILE_PATHS})
    message(STATUS "-----RESULT_RUNTIME_SOURCE_FILE_PATHS: ${RESULT_RUNTIME_SOURCE_FILE_PATHS}-----")
    message(STATUS "-----FIND_AND_GROUP_RUNTIME_SOURCE_FILES: END-----")
    return(PROPAGATE ${RESULT_RUNTIME_SOURCE_FILE_PATHS})
endfunction()

function(ADD_RUNTIME_PACKAGES EXECUTABLE_NAME)
    find_package(assimp CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC assimp::assimp)

    # find_package(BDWgc CONFIG REQUIRED)
    # target_link_libraries("${EXECUTABLE_NAME}" PUBLIC BDWgc::gc BDWgc::cord BDWgc::gccpp BDWgc::gctba)

    # target_include_directories("${EXECUTABLE_NAME}" PUBLIC ${Boost_INCLUDE_DIRS})
    # find_package(Boost COMPONENTS fiber REQUIRED)
    # target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Boost::boost Boost::fiber)

    find_package(cityhash CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC cityhash)

    find_package(glm CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC glm::glm)

    find_package(nlohmann_json CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC nlohmann_json::nlohmann_json)

    find_package(OpenCV REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC ${OpenCV_LIBS})

    set_target_properties("${EXECUTABLE_NAME}" PROPERTIES AUTOMOC ON)
    find_package(Qt6Core CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Qt::Core Qt6::Core Qt::CorePrivate Qt6::CorePrivate)

    find_package(Qt6Gui CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Qt::Gui Qt6::Gui Qt::GuiPrivate Qt6::GuiPrivate)

    find_package(Qt6Widgets CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Qt::Widgets Qt6::Widgets Qt::WidgetsPrivate Qt6::WidgetsPrivate)

    find_package(rttr CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC RTTR::Core_Lib)

    find_package(unofficial-spirv-reflect CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC unofficial::spirv-reflect::spirv-reflect)

    find_package(Taskflow CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Taskflow::Taskflow)

    find_package(vk-bootstrap CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC vk-bootstrap::vk-bootstrap)

    find_package(Vulkan REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC Vulkan::Vulkan)

    find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
    target_link_libraries("${EXECUTABLE_NAME}" PUBLIC unofficial::vulkan-memory-allocator::vulkan-memory-allocator) 
endfunction()