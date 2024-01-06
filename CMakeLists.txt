cmake_minimum_required(VERSION 3.27)

project(cpp2-example)

add_subdirectory(flecs ${CMAKE_CURRENT_BINARY_DIR}/flecs)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)