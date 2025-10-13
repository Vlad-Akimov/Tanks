cmake_minimum_required(VERSION 3.10)
project(TanksGame)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск всех исходных файлов
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Добавление исполняемого файла
add_executable(TanksGame ${SOURCES} ${HEADERS})

# Установка путей для включения заголовков
target_include_directories(TanksGame PRIVATE src)

# Настройки компилятора
if(MSVC)
    target_compile_options(TanksGame PRIVATE /W4)
else()
    target_compile_options(TanksGame PRIVATE -Wall -Wextra)
endif()