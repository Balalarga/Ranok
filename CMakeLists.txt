cmake_minimum_required(VERSION 3.5)

project(Ranok LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THIRDPARTY_DIR ThirdParty)
# Windows libs only
set(LIBS mingw32 opengl32 glu32 Imm32)
set(LIBS_LOCATIONS)
set(LIBS_INCLUDES)

# SDL2 setup
set(SDL2_DIR ${THIRDPARTY_DIR}/SDL2_mingw/x64)
set(LIBS_LOCATIONS ${SDL2_DIR}/lib)
set(LIBS_INCLUDES ${SDL2_DIR}/include/SDL2)
set(LIBS ${LIBS} SDL2main SDL2 SDL2_image SDL2_ttf)
set(BINARIES ${SDL2_DIR}/bin)

# Glew setup
set(GLEW_DIR ${THIRDPARTY_DIR}/GLEW)
set(LIBS_LOCATIONS ${LIBS_LOCATIONS} ${GLEW_DIR}/lib)
set(LIBS_INCLUDES ${LIBS_INCLUDES} ${GLEW_DIR}/include)
set(LIBS ${LIBS} glew32)
set(BINARIES ${BINARIES} ${GLEW_DIR}/bin)

# ImGui setup
add_library(ImGui
    # Main Imgui files
    ${THIRDPARTY_DIR}/ImGui/imgui.cpp ${THIRDPARTY_DIR}/ImGui/imgui_draw.cpp
    ${THIRDPARTY_DIR}/ImGui/imgui_tables.cpp ${THIRDPARTY_DIR}/ImGui/imgui_widgets.cpp
    ${THIRDPARTY_DIR}/ImGui/imgui_demo.cpp
    # SDL2+OpenGL-specific files
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_sdl.cpp
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_opengl3.cpp)
target_link_libraries(ImGui PUBLIC ${LIBS})
target_link_directories(ImGui PUBLIC ${LIBS_LOCATIONS})
target_include_directories(ImGui PUBLIC ${THIRDPARTY_DIR}/ImGui
                           ${THIRDPARTY_DIR}/ImGui/backends
                           ${LIBS_INCLUDES})
set(LIBS ${LIBS} ImGui)

# RanokCore setup
add_subdirectory(ThirdParty/RanokLibraryCore/RanokLibraryCore)
set(LIBS ${LIBS} RanokLibraryCore)

# Find all source files
file(GLOB_RECURSE PROJECT_SOURCES Sources/*.cpp Sources/*.h Sources/*.hpp)
include_directories(Sources)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link all libraries to main project
target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_directories(${PROJECT_NAME} PUBLIC ${LIBS_LOCAIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBS_INCLUDES})

# Copy dll files
foreach(BINARY ${BINARIES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/${BINARY} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()
