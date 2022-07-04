cmake_minimum_required(VERSION 3.5)

project(Ranok LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (!MSVC)
    add_compile_options(-Wl,-subsystem,windows)
endif()

if (MINGW)
    set(LIBS mingw32)
endif()

set(THIRDPARTY_DIR ThirdParty)
# Windows libs only
set(LIBS ${LIBS} opengl32 glu32 Imm32)
set(LIBS_LOCATIONS)
set(LIBS_INCLUDES)

# SDL2 setup
set(SDL_DIR ${THIRDPARTY_DIR}/SDL2)
set(LIBS_INCLUDES ${LIBS_INCLUDES} ${SDL_DIR}/include)
set(BINARIES ${BINARIES} ${SDL_DIR}/bin)
set(LIBS ${LIBS} SDL2 SDL2main SDL2_image SDL2_ttf)
if (MSVC)
    set(LIBS_LOCATIONS ${LIBS_LOCATIONS} ${SDL_DIR}/lib/MSVC/x64)
else()
    set(LIBS_LOCATIONS ${LIBS_LOCATIONS} ${SDL_DIR}/lib/MinGW/x64)
endif()

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
    ${THIRDPARTY_DIR}/NodeEditor/imgui_node_editor.cpp
    ${THIRDPARTY_DIR}/NodeEditor/imgui_node_editor_api.cpp
    ${THIRDPARTY_DIR}/NodeEditor/crude_json.cpp
    ${THIRDPARTY_DIR}/NodeEditor/imgui_canvas.cpp
    ${THIRDPARTY_DIR}/NodeEditor/imgui_node_editor.cpp
    # SDL2+OpenGL-specific files
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_sdl.cpp
    ${THIRDPARTY_DIR}/ImGui/backends/imgui_impl_opengl3.cpp)
target_link_libraries(ImGui PUBLIC ${LIBS})
target_link_directories(ImGui PUBLIC ${LIBS_LOCATIONS})
target_include_directories(ImGui PUBLIC ${THIRDPARTY_DIR}/ImGui
                           ${THIRDPARTY_DIR}/ImGui/backends
                           ${THIRDPARTY_DIR}/NodeEditor
                           ${LIBS_INCLUDES})
set(LIBS ${LIBS} ImGui)

# RanokCalculator setup
add_subdirectory(ThirdParty/RanokCalculator/RanokCalculator)
set(LIBS ${LIBS} RanokCalculator)

# Find all source files
file(GLOB_RECURSE PROJECT_SOURCES Sources/*.cpp Sources/*.h Sources/*.hpp Assets/*)
include_directories(Sources)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link all libraries to main project
target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_directories(${PROJECT_NAME} PUBLIC ${LIBS_LOCAIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBS_INCLUDES})

# Copy assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/Assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets)

# Copy dll files
foreach(BINARY ${BINARIES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        ${CMAKE_SOURCE_DIR}/${BINARY} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endforeach()
