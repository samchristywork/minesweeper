cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(minesweeper VERSION 1.0)

find_package(SDL2 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_library(render STATIC src/render.cpp)

add_executable(main src/main.cpp)

target_link_libraries(main render)
target_link_libraries(main ${SDL2_LIBRARIES})
target_link_libraries(main -lSDL2_ttf -lSDL2_image)
