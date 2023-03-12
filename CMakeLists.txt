cmake_minimum_required(VERSION 3.20)

project(Ranok VERSION 3.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMakeUtils")
include(Utils/FileSystem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(PROJECT_RESOURCE_DIR ${CMAKE_SOURCE_DIR}/Assets)
file(GLOB_RECURSE PROJECT_RESOURCES ${PROJECT_RESOURCE_DIR}/*)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
add_subdirectory(${THIRDPARTY_DIR})

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ThirdParty)
target_link_libraries(${PROJECT_NAME} INTERFACE ThirdParty)

set(MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source)
ListOfDirectChildDirectories(MODULES_LIST ${MODULES_DIR})

foreach(MODULE ${MODULES_LIST})
    if(EXISTS ${MODULES_DIR}/${MODULE}/CMakeLists.txt)
        add_subdirectory(${MODULES_DIR}/${MODULE} ${MODULE})
        target_include_directories(${PROJECT_NAME} INTERFACE ${MODULE})
        target_link_libraries(${PROJECT_NAME} INTERFACE ${MODULE})
        get_property(MODULE_TARGET DIRECTORY ${MODULES_DIR}/${MODULE} PROPERTY BUILDSYSTEM_TARGETS)
        set_target_properties(${MODULE_TARGET} PROPERTIES FOLDER ${PROJECT_NAME})
        message("Added submodule ${MODULE}")
    endif()
endforeach()

set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${PROJECT_RESOURCES}")

# Copy dll files
foreach(BINARY ${THIRDPARTY_BINARIES})
    file(GLOB_RECURSE FILES_TO_COPY ${BINARY}/*)
    file(COPY ${FILES_TO_COPY} DESTINATION "${CMAKE_BINARY_DIR}/Bin/Debug")
    message("${BINARY} DESTINATION ${CMAKE_BINARY_DIR}/Bin/Debug")

    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            # COMMAND ${CMAKE_COMMAND} -E copy_directory
            # ${BINARY} ${PROJECT_BINARY_DIR})
    # add_custom_target(BINARY_COPY
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     ${BINARY} ${PROJECT_BINARY_DIR})
endforeach()