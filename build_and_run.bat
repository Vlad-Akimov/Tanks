@echo off
chcp 65001 >nul
echo Компиляция Tank Game...

:: Создаем папку для объектных файлов если ее нет
if not exist build mkdir build

:: Компилируем все файлы
g++ -std=c++17 -Isrc -o build/tank_game.exe ^
    src/main.cpp ^
    src/model/GameObject.cpp ^
    src/model/Tank.cpp ^
    src/model/PlayerTank.cpp ^
    src/model/EnemyTank.cpp ^
    src/model/Obstacle.cpp ^
    src/model/Bonus.cpp ^
    src/model/Projectile.cpp ^
    src/model/GameWorld.cpp ^
    src/view/ConsoleRenderer.cpp ^
    src/controller/GameController.cpp

if %errorlevel% equ 0 (
    echo Компиляция успешно завершена!
    echo Запуск игры...
    build\tank_game.exe
) else (
    echo Ошибка компиляции!
    pause
)