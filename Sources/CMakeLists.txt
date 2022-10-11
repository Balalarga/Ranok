cmake_minimum_required(VERSION 3.20)

project(Ranok LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(PROJECT_RESOURCE_DIR ${CMAKE_SOURCE_DIR}/Assets)
file(GLOB_RECURSE PROJECT_RESOURCES ${PROJECT_RESOURCE_DIR}/*)
file(GLOB_RECURSE PROJECT_SOURCES ./*.cpp ./*.h ./*.hpp)

# ImGui setup
set(ADDITIONAL_SOURCES
    # Main Imgui files
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_sdl.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_opengl3.cpp
    # Additional ImGui subprojects:
    # 1. FileDialog
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGuiFileDialog-0.6.4/ImGuiFileDialog.cpp
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGuiFileDialog-0.6.4/ImGuiFileDialog.h)
    
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGui/backends/
    ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/ImGuiFileDialog-0.6.4/)

# Glew setup
set(GLEW_DIR ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/GLEW)
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${GLEW_DIR}/include)
set(ADDITIONAL_LINK_DIRS ${GLEW_DIR}/lib)
set(BINARIES ${GLEW_DIR}/bin)

# GLM includes
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/glm)

# SDL setup
set(SDL_DIR ${CMAKE_SOURCE_DIR}/${THIRDPARTY_DIR}/SDL2)
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${SDL_DIR}/include)
set(BINARIES ${BINARIES} ${SDL_DIR}/bin)
set(LIBS ${LIBS} SDL2 SDL2main SDL2_image SDL2_ttf)
if (MSVC)
    set(ADDITIONAL_LINK_DIRS ${ADDITIONAL_LINK_DIRS} ${SDL_DIR}/lib/MSVC/x64)
elseif(MINGW OR UNIX)
    set(ADDITIONAL_LINK_DIRS ${ADDITIONAL_LINK_DIRS} ${SDL_DIR}/lib/MinGW/x64)
endif()

# OpenCL
find_package(OpenCL REQUIRED)
set(ADDITIONAL_INCLUDE_DIRS ${ADDITIONAL_INCLUDE_DIRS} ${OpenCL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OpenCL_LIBRARY})

# RapidJson
# set(LIBS ${LIBS} RapidJson)

# Fmt
set(LIBS ${LIBS} fmt)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${ADDITIONAL_SOURCES} ${PROJECT_RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${ADDITIONAL_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${ADDITIONAL_LINK_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBS})

target_compile_definitions(${PROJECT_NAME} PRIVATE "RESOURCE_DIR=\"${PROJECT_RESOURCE_DIR}\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE "SDL_LIBRARY_DIR=\"${PROJECT_RESOURCE_DIR}/StdLibrary\"")
set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${PROJECT_RESOURCES}")


# Copy dll files
foreach(BINARY ${BINARIES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${BINARY} ${PROJECT_BINARY_DIR})
endforeach()