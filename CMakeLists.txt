cmake_minimum_required(VERSION 3.27)

project(cpp2-example)

find_package(flecs CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:flecs::flecs>,flecs::flecs,flecs::flecs_static>)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)