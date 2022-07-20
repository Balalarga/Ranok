cmake_minimum_required(VERSION 3.20)

project(Language LANGUAGES CXX)


file(GLOB_RECURSE PROJECT_SOURCES ./*.cpp ./*.h ./*.hpp)
add_library(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC fmt)