@echo off
chcp 65001 >nul
echo Быстрая компиляция Tank Game...

g++ -std=c++17 -Isrc -o build/tank_game.exe ^
    src/main.cpp ^
    src/model/*.cpp ^
    src/view/*.cpp ^
    src/controller/*.cpp

if %errorlevel% equ 0 (
    echo Компиляция завершена успешно!
) else (
    echo Ошибка компиляции!
)
pause