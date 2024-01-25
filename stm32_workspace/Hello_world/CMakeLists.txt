cmake_minimum_required(VERSION 3.20)

project("Hello_world" C CXX ASM)

include(cmake/st-project.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})