cmake_minimum_required(VERSION 3.5.1)
project(WAVE_LAB C)

set(SRC src/main.c src/maze_gen.c)  # + maze_gen.c

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(common_lab)
