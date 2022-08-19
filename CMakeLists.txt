cmake_minimum_required(VERSION 3.20)

project(Ranok VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(THIRDPARTY_DIR ThirdParty)

if (WIN32)
    set(LIBS glew32 opengl32 glu32 Imm32)
    if (MINGW)
        set(LIBS ${LIBS} mingw32)
    endif()
elseif (UNIX)
    set(LIBS GLEW GL GLU)
endif()

# Fmt setup
add_subdirectory(${THIRDPARTY_DIR}/fmt fmt)


add_subdirectory(Sources)