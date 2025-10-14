#ifndef CONSOLERENDERER_CPP
#define CONSOLERENDERER_CPP

#include "ConsoleRenderer.h"
#include <iostream>
#include <string>
#include <vector>

// Реализация ConsoleRenderer

ConsoleRenderer::ConsoleRenderer(int width, int height): screenWidth(width), screenHeight(height) {}

void ConsoleRenderer::setColor(PlatformUtils::Color color) {
    PlatformUtils::setColor(color);
}

void ConsoleRenderer::resetColor() {
    PlatformUtils::resetColor();
}

void ConsoleRenderer::drawBorder() {
    // Верхняя граница
    std::cout << "+";
    for (int i = 0; i < screenWidth; i++) { std::cout << "-"; }
    std::cout << "+\n";
}

void ConsoleRenderer::clearScreen() {
    PlatformUtils::clearScreen();
}

void ConsoleRenderer::setCursorPosition(int x, int y) {
    PlatformUtils::setCursorPosition(x, y);
}

void ConsoleRenderer::render(const GameWorld& world) {
    // Рисуем верхнюю границу с информацией
    std::cout << "Уровень: " << world.getCurrentLevel() 
              << " | Очки: " << world.getPlayer()->getScore()
              << " | Жизни: " << world.getPlayer()->getLives()
              << " | Здоровье: " << world.getPlayer()->getHealth() << "\n";
    
    drawBorder();
    
    // Создаем буфер для отрисовки
    std::vector<std::vector<char>> buffer(screenHeight, std::vector<char>(screenWidth, ' '));
    
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
    
    // Выводим буфер на экран с цветами
    for (int y = 0; y < screenHeight; y++) {
        std::cout << "|"; // Левая граница
        for (int x = 0; x < screenWidth; x++) { 
            char symbol = buffer[y][x];
            
            // Определяем цвет для символа
            bool colored = false;
            
            // Проверяем, является ли символ танком игрока
            if (symbol == '^' || symbol == 'v' || symbol == '<' || symbol == '>') {
                setColor(PlatformUtils::Color::GREEN);
                colored = true;
            }
            // Проверяем, является ли символ вражеским танком
            else if (symbol == 'A' || symbol == 'V' || symbol == '[' || symbol == ']' || symbol == 'E') {
                setColor(PlatformUtils::Color::RED);
                colored = true;
            }
            // Проверяем, является ли символ бонусом
            else if (symbol == 'S' || symbol == 'F' || symbol == 'B' || symbol == 'L') {
                setColor(PlatformUtils::Color::YELLOW);
                colored = true;
            }
            // Проверяем, является ли символ снарядом
            else if (symbol == '|' || symbol == '-') {
                setColor(PlatformUtils::Color::WHITE);
                colored = true;
            }
            // Проверяем, является ли символ препятствием
            else if (symbol == '#' || symbol == 'X') {
                colored = true;
            }
            // Проверяем, является ли символ водой или лесом
            else if (symbol == '~' || symbol == '*') {
                setColor(PlatformUtils::Color::BLUE);
                colored = true;
            }
            
            std::cout << symbol;
            
            // Сбрасываем цвет после вывода символа
            if (colored) {
                resetColor();
            }
        }
        std::cout << "|\n"; // Правая граница
    }
    
    drawBorder();
    
    // Отображаем статус бонусов игрока
    PlayerTank* player = world.getPlayer();
    if (player) {
        std::cout << "Бонусы: ";
        if (player->getHasShield()) { std::cout << "[Щит] "; }
        if (player->getDoubleFire()) { std::cout << "[Двойной огонь] "; }
        if (player->getBonusDuration() > 0) { std::cout << "(осталось ходов: " << player->getBonusDuration() << ")"; }
        std::cout << "\n";
    }
    
    // Отображаем легенду символов
    drawSymbolLegend();
}

void ConsoleRenderer::drawSymbolLegend() {
    std::cout << "\n=== ЛЕГЕНДА СИМВОЛОВ ===\n";
    
    // Танки
    std::cout << "Танки: ";
    setColor(PlatformUtils::Color::GREEN);
    std::cout << "^ v < >";
    resetColor();
    std::cout << " - игрок, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "A V [ ]";
    resetColor();
    std::cout << " - враги\n";
    
    // Снаряды
    std::cout << "Снаряды: ";
    setColor(PlatformUtils::Color::WHITE);
    std::cout << "| -";
    resetColor();
    std::cout << " (верт./гориз.)\n";
    
    // Препятствия
    std::cout << "Препятствия: ";
    std::cout << "#";
    resetColor();
    std::cout << " - кирпич, ";
    std::cout << "X";
    resetColor();
    std::cout << " - сталь, ";
    setColor(PlatformUtils::Color::BLUE);
    std::cout << "~";
    resetColor();
    std::cout << " - вода, ";
    setColor(PlatformUtils::Color::BLUE);
    std::cout << "*";
    resetColor();
    std::cout << " - лес\n";
    
    // Бонусы
    std::cout << "Бонусы: ";
    setColor(PlatformUtils::Color::YELLOW);
    std::cout << "S";
    resetColor();
    std::cout << " - щит, ";
    setColor(PlatformUtils::Color::YELLOW);
    std::cout << "F";
    resetColor();
    std::cout << " - двойной огонь, ";
    setColor(PlatformUtils::Color::YELLOW);
    std::cout << "B";
    resetColor();
    std::cout << " - скорость, ";
    setColor(PlatformUtils::Color::YELLOW);
    std::cout << "L";
    resetColor();
    std::cout << " - +1 жизнь\n";
}

void ConsoleRenderer::drawMenu() {
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
    
    // Добавляем легенду символов в меню
    std::cout << "---------------------------------\n";
    std::cout << "        ОБОЗНАЧЕНИЯ В ИГРЕ       \n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "Игрок: ^ v < >  |  Враги: A V [ ]\n";
    std::cout << "Снаряды: | -  |  Стены: # X\n";
    std::cout << "Вода: ~  |  Лес: *  |  Бонусы: S F B L\n\n";
    
    std::cout << "=================================\n";
    std::cout << "   Пошаговый режим - жми ENTER  \n";
    std::cout << "     для завершения каждого хода\n";
    std::cout << "=================================\n";
}

void ConsoleRenderer::drawPauseScreen() {
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
    
    // Добавляем краткую легенду на экране паузы
    std::cout << "Краткая справка:\n";
    std::cout << "Игрок: ^>v<  Враги: AV[]  Бонусы: SFBL\n";
    std::cout << "Стены: #X  Вода: ~  Лес: *\n\n";
    
    std::cout << "=================================\n";
}

void ConsoleRenderer::drawGameOver(int score) {
    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "         ИГРА ОКОНЧЕНА          \n";
    std::cout << "=================================\n\n";
    
    std::cout << "    Ваш счет: " << score << "\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "   [ENTER] - Новая игра\n";
    std::cout << "   [M] - Главное меню\n";
    std::cout << "   [Q] - Выход\n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "=================================\n";
    std::cout << "  Спасибо за игру!              \n";
    std::cout << "=================================\n";
}

void ConsoleRenderer::drawSettings() {
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
    
    // Добавляем раздел с обозначениями
    std::cout << "   Обозначения в игре:\n\n";
    std::cout << "   Игрок: ^ v < >\n";
    std::cout << "   Враги: A V [ ]\n";
    std::cout << "   Снаряды: | -\n";
    std::cout << "   Кирпич: #  Сталь: X\n";
    std::cout << "   Вода: ~  Лес: *\n";
    std::cout << "   Бонусы: S(щит) F(огонь) B(скорость) L(жизнь)\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "   [ESC] - Назад\n";
    std::cout << "   [Q] - Выход\n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "=================================\n";
    std::cout << "  Настройки сохраняются автоматически\n";
    std::cout << "=================================\n";
}

#endif // CONSOLERENDERER_CPP
