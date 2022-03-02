cmake_minimum_required(VERSION 3.5)

project(Ranok LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRDPARTY_DIR ThirdParty)
# Windows libs only
set(LIBS mingw32 opengl32 glu32 Imm32)

# SDL2 setup
set(SDL2_DIR ${THIRDPARTY_DIR}/SDL2_mingw/x64)
link_directories(${SDL2_DIR}/lib)
include_directories(${SDL2_DIR}/include/SDL2)
set(LIBS ${LIBS} SDL2main SDL2 SDL2_image SDL2_ttf)
set(BINARIES ${SDL2_DIR}/bin)

# Glew setup
set(GLEW_DIR ${THIRDPARTY_DIR}/GLEW)
link_directories(${GLEW_DIR}/lib)
include_directories(${GLEW_DIR}/include)
set(LIBS ${LIBS} glew32)
set(BINARIES ${GLEW_DIR}/bin)

# RanokCore setup
add_subdirectory(ThirdParty/RanokCore/RanokCoreLib)
set(LIBS ${LIBS} RanokCore)

# ImGui setup
add_library(ImGui
    # Main Imgui files
    ${THIRDPARTY_DIR}/ImGui/imgui.cpp ${THIRDPARTY_DIR}/ImGui/imgui_draw.cpp
    ${THIRDPARTY_DIR}/ImGui/imgui_tables.cpp ${THIRDPARTY_DIR}/ImGui/imgui_widgets.cpp
    ${THIRDPARTY_DIR}/ImGui/imgui_demo.cpp
    # SDL2+OpenGL-specific files
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_sdl.cpp
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_opengl3.cpp)
target_include_directories(ImGui PUBLIC ${THIRDPARTY_DIR}/ImGui/ ${THIRDPARTY_DIR}/ImGui/backends/)
target_link_libraries(ImGui ${LIBS})
set(LIBS ${LIBS} ImGui)

# Find all source files
file(GLOB_RECURSE PROJECT_SOURCES Sources/*.cpp Sources/*.h Sources/*.hpp)
include_directories(Sources)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link all libraries to main project
target_link_libraries(${PROJECT_NAME} ${LIBS})

# Copy dll files
foreach(BINARY ${BINARIES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/${BINARY} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()
