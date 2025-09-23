# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(TanksDendyGame)

set(CMAKE_CXX_STANDARD 17)

# Настройка компилятора
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Поиск SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Исходные файлы
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Создание исполняемого файла
add_executable(TanksDendyGame ${SOURCES} ${HEADERS})

# Подключение SFML
target_link_libraries(TanksDendyGame sfml-graphics sfml-window sfml-system)

# Включение директорий
target_include_directories(TanksDendyGame PRIVATE src)

# Копирование ресурсов
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET TanksDendyGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:TanksDendyGame>/resources)
endif()