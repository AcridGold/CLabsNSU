cmake_minimum_required(VERSION 3.15)
project(3d-lab)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# Укажите путь к SDL3
set(SDL3_PATH "SDL-lib/x86_64-w64-mingw32")

# Подключение директорий заголовочных файлов SDL3
include_directories(${SDL3_PATH}/include)

# Подключение библиотек SDL3
link_directories(${SDL3_PATH}/lib)

# Исходные файлы проекта
set(SOURCE_FILES main.c)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Линковка с SDL3
target_link_libraries(${PROJECT_NAME} SDL3)