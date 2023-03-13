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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Libs") # .lib and .a
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Libs") # .so and .dylib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}") # .exe and .dll

add_compile_definitions("PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")
add_compile_definitions("OVERRIDE_DEFAULT_CONFIG=1")
add_compile_definitions("RANOK_CODE_EXTENSION=\".rcode\"")

set(PROJECT_RESOURCE_DIR ${CMAKE_SOURCE_DIR}/Assets)
file(GLOB_RECURSE PROJECT_RESOURCES ${PROJECT_RESOURCE_DIR}/*)
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
add_subdirectory(${THIRDPARTY_DIR})

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ThirdParty)
target_link_libraries(${PROJECT_NAME} INTERFACE ThirdParty)
set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${PROJECT_RESOURCES}")

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

# Copy dll files
foreach(BINARY ${THIRDPARTY_BINARIES})
    file(GLOB_RECURSE FILES_TO_COPY ${BINARY}/*)
    file(COPY ${FILES_TO_COPY} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
endforeach()
