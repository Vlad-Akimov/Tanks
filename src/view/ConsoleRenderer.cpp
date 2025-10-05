#ifndef CONSOLERENDERER_CPP
#define CONSOLERENDERER_CPP

#include "../model/GameWorld.cpp"
#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

class ConsoleRenderer {
private:
    int screenWidth, screenHeight;

    void drawBorder() {
        // Верхняя граница
        std::cout << "+";
        for (int i = 0; i < screenWidth; i++) {
            std::cout << "-";
        }
        std::cout << "+\n";
    }

public:
    ConsoleRenderer(int width, int height) 
        : screenWidth(width), screenHeight(height) {}
    
    // Вспомогательные методы для работы с консолью
    void clearScreen() {
        #ifdef _WIN32
                system("cls");
        #else
                system("clear");
        #endif
    }

    void setCursorPosition(int x, int y) {
        #ifdef _WIN32
                COORD coord;
                coord.X = x;
                coord.Y = y;
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        #else
                std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
        #endif
    }

    // Основные методы отрисовки
    void render(const GameWorld& world) {
        clearScreen();
        
        // Рисуем верхнюю границу с информацией
        std::cout << "Уровень: " << world.getCurrentLevel() 
                  << " | Очки: " << world.getPlayer()->getScore()
                  << " | Жизни: " << world.getPlayer()->getLives()
                  << " | Здоровье: " << world.getPlayer()->getHealth() << "\n";
        
        drawBorder();
        
        // Создаем буфер для отрисовки
        std::vector<std::vector<char>> buffer(screenHeight, 
                                            std::vector<char>(screenWidth, ' '));
        
        // Заполняем буфер объектами
        for (const auto& obj : world.getObjects()) {
            if (obj->isDestroyed()) continue;
            
            Point pos = obj->getPosition();
            Point bounds = obj->getBounds();
            char symbol = obj->getSymbol();
            
            // Рисуем объект в буфере
            for (int y = 0; y < bounds.y; y++) {
                for (int x = 0; x < bounds.x; x++) {
                    int drawX = pos.x + x;
                    int drawY = pos.y + y;
                    
                    if (drawX >= 0 && drawX < screenWidth && 
                        drawY >= 0 && drawY < screenHeight) {
                        buffer[drawY][drawX] = symbol;
                    }
                }
            }
        }
        
        // Рисуем снаряды
        for (const auto& proj : world.getProjectiles()) {
            if (proj->isDestroyed()) continue;
            
            Point pos = proj->getPosition();
            if (pos.x >= 0 && pos.x < screenWidth && 
                pos.y >= 0 && pos.y < screenHeight) {
                buffer[pos.y][pos.x] = proj->getSymbol();
            }
        }
        
        // Рисуем бонусы
        for (const auto& bonus : world.getBonuses()) {
            if (!bonus->isActive()) continue;
            
            Point pos = bonus->getPosition();
            if (pos.x >= 0 && pos.x < screenWidth && 
                pos.y >= 0 && pos.y < screenHeight) {
                buffer[pos.y][pos.x] = bonus->getSymbol();
            }
        }
        
        // Выводим буфер на экран
        for (int y = 0; y < screenHeight; y++) {
            std::cout << "|"; // Левая граница
            for (int x = 0; x < screenWidth; x++) {
                std::cout << buffer[y][x];
            }
            std::cout << "|\n"; // Правая граница
        }
        
        drawBorder();
        
        // Отображаем статус бонусов игрока
        PlayerTank* player = world.getPlayer();
        if (player) {
            std::cout << "Бонусы: ";
            if (player->getHasShield()) {
                std::cout << "[Щит] ";
            }
            if (player->getDoubleFire()) {
                std::cout << "[Двойной огонь] ";
            }
            if (player->getBonusDuration() > 0) {
                std::cout << "(осталось ходов: " << player->getBonusDuration() << ")";
            }
            std::cout << "\n";
        }
    }
    
    void drawMenu() {
        clearScreen();
        
        std::cout << "=================================\n";
        std::cout << "        ТАНКОВЫЙ СИМУЛЯТОР       \n";
        std::cout << "=================================\n\n";
        
        std::cout << "         КОНСОЛЬНАЯ ВЕРСИЯ       \n\n";
        
        std::cout << "---------------------------------\n";
        std::cout << "          ГЛАВНОЕ МЕНЮ          \n";
        std::cout << "---------------------------------\n\n";
        
        std::cout << "   [ENTER] - Начать игру\n";
        std::cout << "   [M] - Настройки\n";
        std::cout << "   [Q] - Выход\n\n";
        
        std::cout << "---------------------------------\n";
        std::cout << "        УПРАВЛЕНИЕ В ИГРЕ       \n";
        std::cout << "---------------------------------\n\n";
        
        std::cout << "   Движение: WASD или стрелки\n";
        std::cout << "   Выстрел: SPACE или F\n";
        std::cout << "   Пауза: P\n";
        std::cout << "   Меню: M\n";
        std::cout << "   Выход: Q\n\n";
        
        std::cout << "=================================\n";
        std::cout << "   Пошаговый режим - жми ENTER  \n";
        std::cout << "     для завершения каждого хода\n";
        std::cout << "=================================\n";
    }
    
    void drawPauseScreen() {
        clearScreen();
        
        std::cout << "=================================\n";
        std::cout << "           ПАУЗА                \n";
        std::cout << "=================================\n\n";
        
        std::cout << "    Игра приостановлена\n\n";
        
        std::cout << "---------------------------------\n";
        std::cout << "   [P] или [ENTER] - Продолжить\n";
        std::cout << "   [M] - Главное меню\n";
        std::cout << "   [Q] - Выход\n";
        std::cout << "---------------------------------\n\n";
        
        std::cout << "=================================\n";
    }
    
    void drawGameOver(int score) {
        clearScreen();
        
        std::cout << "=================================\n";
        std::cout << "         ИГРА ОКОНЧЕНА          \n";
        std::cout << "=================================\n\n";
        
        std::cout << "    Ваш счет: " << score << "\n\n";
        
        std::cout << "---------------------------------\n";
        std::cout << "   [M] - Главное меню\n";
        std::cout << "   [Q] - Выход\n";
        std::cout << "---------------------------------\n\n";
        
        std::cout << "=================================\n";
        std::cout << "  Спасибо за игру!              \n";
        std::cout << "=================================\n";
    }
    
    void drawSettings() {
        clearScreen();
        
        std::cout << "=================================\n";
        std::cout << "           НАСТРОЙКИ            \n";
        std::cout << "=================================\n\n";
        
        std::cout << "   Настройки управления:\n\n";
        
        std::cout << "   Движение вверх: W / Стрелка вверх\n";
        std::cout << "   Движение вниз: S / Стрелка вниз\n";
        std::cout << "   Движение влево: A / Стрелка влево\n";
        std::cout << "   Движение вправо: D / Стрелка вправо\n";
        std::cout << "   Выстрел: SPACE / F\n";
        std::cout << "   Пауза: P\n";
        std::cout << "   Меню: M\n\n";
        
        std::cout << "   Настройки игры:\n\n";
        
        std::cout << "   Размер поля: " << screenWidth << "x" << screenHeight << "\n";
        std::cout << "   Пошаговый режим: ВКЛ\n";
        std::cout << "   Управление: Клавиатура\n\n";
        
        std::cout << "---------------------------------\n";
        std::cout << "   [ESC] - Назад\n";
        std::cout << "   [Q] - Выход\n";
        std::cout << "---------------------------------\n\n";
        
        std::cout << "=================================\n";
        std::cout << "  Настройки сохраняются автоматически\n";
        std::cout << "=================================\n";
    }
};

#endif // CONSOLERENDERER_CPP