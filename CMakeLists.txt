cmake_minimum_required(VERSION 3.20)

project(Ranok VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32 AND MINGW)
    set(LIBS mingw32)
endif()

set(THIRDPARTY_DIR ThirdParty)
add_subdirectory(${THIRDPARTY_DIR}/fmt fmt)

add_subdirectory(Sources)