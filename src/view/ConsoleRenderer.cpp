/**
 * @file ConsoleRenderer.cpp
 * @author Vld251
 * @brief Implementation of console-based game rendering with ASCII/Unicode support.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ConsoleRenderer.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

ConsoleRenderer::ConsoleRenderer() 
    : screenWidth(40), screenHeight(20), terminalSizeValid(true),
    useAdvancedGraphics(true) {
    updateTerminalSize();
}

void ConsoleRenderer::setAdvancedGraphics(bool enabled) {
    useAdvancedGraphics = enabled;
}

bool ConsoleRenderer::getAdvancedGraphics() const {
    return useAdvancedGraphics;
}

bool ConsoleRenderer::isTerminalSizeValid() const {
    return terminalSizeValid;
}

void ConsoleRenderer::updateTerminalSize() {
    auto size = PlatformUtils::getTerminalSize();
    terminalWidth = size.first;
    terminalHeight = size.second;
    
    int requiredWidth = 60;
    int requiredHeight = 40;
    
    terminalSizeValid = (terminalWidth >= requiredWidth && terminalHeight >= requiredHeight);
}

bool ConsoleRenderer::checkTerminalSize() {
    updateTerminalSize();
    return terminalSizeValid;
}

void ConsoleRenderer::drawCenteredText(const std::string& text, int width) {
    int padding = (width - text.length()) / 2;
    if (padding > 0) {
        std::cout << std::string(padding, ' ');
    }
    std::cout << text;
    if (padding > 0) {
        std::cout << std::string(padding, ' ');
    }
}

void ConsoleRenderer::setColor(PlatformUtils::Color color) {
    PlatformUtils::setColor(color);
}

void ConsoleRenderer::resetColor() {
    PlatformUtils::resetColor();
}

void ConsoleRenderer::drawPixel(char symbol, PlatformUtils::Color color) {
    setColor(color);
    std::cout << symbol;
    resetColor();
}

void ConsoleRenderer::drawBorder() {
    // Верхняя граница
    std::cout << "■";
    for (int i = 0; i < screenWidth; i++) { std::cout << "■"; }
    std::cout << "■\n";
}

void ConsoleRenderer::clearScreen() {
    PlatformUtils::clearScreen();
}

void ConsoleRenderer::setCursorPosition(int x, int y) {
    PlatformUtils::setCursorPosition(x, y);
}

void ConsoleRenderer::drawErrorMessage(const std::string& message) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "============================================\n";
    drawCenteredText("DISPLAY ERROR", terminalWidth);
    std::cout << "\n";
    std::cout << "============================================\n\n";

    drawCenteredText(message, terminalWidth);
    std::cout << "\n\n";
    std::cout << "Current size: " << terminalWidth << "x" << terminalHeight << "\n";
    std::cout << "Required size: " << 60 << "x" << 40 << "\n\n";
    std::cout << "============================================\n";
    drawCenteredText("Increase window size", terminalWidth);
    std::cout << "\n";
    std::cout << "============================================\n";
}

std::map<char, std::pair<std::string, PlatformUtils::Color>> getGraphicsMap(bool useUnicode, bool useAdvancedGraphics) {
    if (useUnicode && useAdvancedGraphics) {
        // Unicode символы
        return {
            {'^', {"▲", PlatformUtils::Color::GREEN}},
            {'v', {"▼", PlatformUtils::Color::GREEN}},
            {'<', {"◄", PlatformUtils::Color::GREEN}},
            {'>', {"►", PlatformUtils::Color::GREEN}},
        
            {'A', {"A", PlatformUtils::Color::RED}},
            {'E', {"E", PlatformUtils::Color::RED}},
            {'F', {"F", PlatformUtils::Color::RED}},
            {'D', {"D", PlatformUtils::Color::RED}},
        
            {'S', {"★", PlatformUtils::Color::MAGENTA}},
            {'K', {"¶", PlatformUtils::Color::MAGENTA}},
            {'B', {"➤", PlatformUtils::Color::MAGENTA}},
            {'L', {"❤", PlatformUtils::Color::MAGENTA}},
        
            {'#', {"█", PlatformUtils::Color::DEFAULT}},
            {'X', {"▓", PlatformUtils::Color::DEFAULT}},
            
            {'~', {"░", PlatformUtils::Color::BLUE}},
            {'*', {"§", PlatformUtils::Color::GREEN}},
            
            {'O', {"●", PlatformUtils::Color::YELLOW}},
            {' ', {" ", PlatformUtils::Color::DEFAULT}}
        };
    } else {
        // ASCII фолбэк
        return {
            {'^', {"^", PlatformUtils::Color::GREEN}},
            {'v', {"v", PlatformUtils::Color::GREEN}},
            {'<', {"<", PlatformUtils::Color::GREEN}},
            {'>', {">", PlatformUtils::Color::GREEN}},

            {'A', {"A", PlatformUtils::Color::RED}},
            {'E', {"E", PlatformUtils::Color::RED}},
            {'F', {"F", PlatformUtils::Color::RED}},
            {'D', {"D", PlatformUtils::Color::RED}},
            
            {'S', {"S", PlatformUtils::Color::MAGENTA}},
            {'K', {"K", PlatformUtils::Color::MAGENTA}},
            {'B', {"B", PlatformUtils::Color::MAGENTA}},
            {'L', {"L", PlatformUtils::Color::MAGENTA}},
            
            {'|', {"|", PlatformUtils::Color::WHITE}},
            {'-', {"-", PlatformUtils::Color::WHITE}},
            
            {'#', {"#", PlatformUtils::Color::DEFAULT}},
            {'X', {"X", PlatformUtils::Color::DEFAULT}},
            
            {'~', {"~", PlatformUtils::Color::BLUE}},
            {'*', {"*", PlatformUtils::Color::GREEN}},
            
            {'O', {"@", PlatformUtils::Color::YELLOW}},
            {' ', {" ", PlatformUtils::Color::DEFAULT}}
        };
    }
}

bool ConsoleRenderer::render(const GameWorld& world) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Размер терминала слишком мал для отображения игры");
        return false;
    }

    bool damageFlashActive = world.isDamageFlashActive();
    bool useUnicode = PlatformUtils::supportsUnicode();
    auto graphicsMap = getGraphicsMap(useUnicode, useAdvancedGraphics);

    // Рисуем верхнюю границу с информацией
    std::cout << "Level: " << world.getCurrentLevel() 
          << " | Score: " << world.getPlayer()->getScore()
          << " | Lives: " << world.getPlayer()->getLives()
          << " | Health: " << world.getPlayer()->getHealth() << "\n";
    
    setColor(PlatformUtils::Color::BLACK);
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    drawBorder();
    resetColor();
    
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

    // Рисуем взрывы
    for (const auto& explosion : world.getExplosions()) {
        if (explosion->isDestroyed()) continue;
        
        Point pos = explosion->getPosition();
        if (pos.x >= 0 && pos.x < screenWidth && 
            pos.y >= 0 && pos.y < screenHeight) {
            buffer[pos.y][pos.x] = explosion->getSymbol();
        }
    }
    
    // Выводим буфер на экран с улучшенной графикой
    for (int y = 0; y < screenHeight; y++) {
        setColor(PlatformUtils::Color::BLACK);
        if (damageFlashActive) {
            setColor(PlatformUtils::Color::RED);
        }
        std::cout << "█"; // Левая граница
        resetColor();
        
        for (int x = 0; x < screenWidth; x++) { 
            char symbol = buffer[y][x];
            
            // Используем графическую карту для улучшенного отображения
            auto it = graphicsMap.find(symbol);
            if (it != graphicsMap.end()) {
                setColor(it->second.second);
                std::cout << it->second.first;
            } else {
                // Для неизвестных символов используем стандартное отображение
                std::cout << symbol;
            }
            
            resetColor();
        }

        setColor(PlatformUtils::Color::BLACK);
        if (damageFlashActive) {
            setColor(PlatformUtils::Color::RED);
        }
        std::cout << "█\n"; // Правая граница
        resetColor();
    }
    
    setColor(PlatformUtils::Color::BLACK);
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    drawBorder();
    resetColor();
    
    // Отображаем статус бонусов игрока
    PlayerTank* player = world.getPlayer();
    if (player) {
        std::cout << "Bonuses: ";
        if (player->getHasShield()) { 
            setColor(PlatformUtils::Color::CYAN);
            std::cout << "[★ Shield] ";
            resetColor();
        }
        if (player->getDoubleFire()) { 
            setColor(PlatformUtils::Color::YELLOW);
            std::cout << "[¶ Double Fire] ";
            resetColor();
        }
        if (player->getBonusDuration() > 0) { 
            std::cout << "(turns left: " << player->getBonusDuration() << ")";
        }
        std::cout << "\n";
    }
    
    drawSymbolLegend();

    return true;
}

void ConsoleRenderer::drawSymbolLegend() {
    bool useUnicode = PlatformUtils::supportsUnicode();
    auto graphicsMap = getGraphicsMap(useUnicode, useAdvancedGraphics);

    std::cout << "\n=== SYMBOL LEGEND ===\n";
    
    // Танки
    std::cout << "Tanks: ";
    setColor(PlatformUtils::Color::GREEN);
    std::cout << "▲▼◄►";
    resetColor();
    std::cout << " - player\n";
    
    std::cout << "Enemies: ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "E";
    resetColor();
    std::cout << " - normal, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "F";
    resetColor();
    std::cout << " - fast, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "D";
    resetColor();
    std::cout << " - strong, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "A";
    resetColor();
    std::cout << " - armored\n";
    
    // Препятствия
    std::cout << "Obstacles: ";
    std::cout << graphicsMap.find('#')->second.first;
    std::cout << " - brick, ";
    std::cout << graphicsMap.find('X')->second.first;
    std::cout << " - steel, ";
    setColor(PlatformUtils::Color::BLUE);
    std::cout << graphicsMap.find('~')->second.first;
    resetColor();
    std::cout << " - water, ";
    setColor(PlatformUtils::Color::GREEN);
    std::cout << graphicsMap.find('*')->second.first;
    resetColor();
    std::cout << " - forest\n";
    
    // Бонусы
    std::cout << "Bonuses: ";
    setColor(PlatformUtils::Color::MAGENTA);
    std::cout << graphicsMap.find('S')->second.first;
    resetColor();
    std::cout << " - shield, ";
    setColor(PlatformUtils::Color::MAGENTA);
    std::cout << graphicsMap.find('K')->second.first;
    resetColor();
    std::cout << " - double fire, ";
    setColor(PlatformUtils::Color::MAGENTA);
    std::cout << graphicsMap.find('B')->second.first;
    resetColor();
    std::cout << " - speed, ";
    setColor(PlatformUtils::Color::MAGENTA);
    std::cout << graphicsMap.find('L')->second.first;
    resetColor();
    std::cout << " - +1 life\n";

    std::cout << "Explosions: ";
    setColor(PlatformUtils::Color::YELLOW);
    std::cout << graphicsMap.find('O')->second.first;
    resetColor();
    std::cout << " - explosion\n";

}

bool ConsoleRenderer::drawMenu() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "        TANK SIMULATOR          \n";
    std::cout << "=================================\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "          MAIN MENU             \n";
    std::cout << "---------------------------------\n\n";
    std::cout << "   [ENTER] - Start Game\n";
    std::cout << "   [M] - Settings\n";
    std::cout << "   [Q] - Exit\n\n";
    std::cout << "---------------------------------\n";
    std::cout << "        IN-GAME CONTROLS        \n";
    std::cout << "---------------------------------\n\n";
    std::cout << "   Movement: WASD or arrow keys\n";
    std::cout << "   Fire: SPACE or F\n";
    std::cout << "   Pause: P\n";
    std::cout << "   Menu: M\n";
    std::cout << "   Exit: Q\n\n";

    return true;
}

bool ConsoleRenderer::drawPauseScreen() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "            PAUSED              \n";
    std::cout << "=================================\n\n";
    std::cout << "    Game is paused\n\n";
    std::cout << "---------------------------------\n";
    std::cout << "   [P] or [ENTER] - Continue\n";
    std::cout << "   [M] - Main Menu\n";
    std::cout << "   [Q] - Exit\n";
    std::cout << "---------------------------------\n\n";

    std::cout << "=================================\n";

    return true;
}

bool ConsoleRenderer::drawGameOver(int score) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "          GAME OVER             \n";
    std::cout << "=================================\n\n";
    std::cout << "    Your score: " << score << "\n\n";
    std::cout << "---------------------------------\n";
    std::cout << "   [ENTER] - New Game\n";
    std::cout << "   [M] - Main Menu\n";
    std::cout << "   [Q] - Exit\n";
    std::cout << "---------------------------------\n\n";

    std::cout << "=================================\n";
    std::cout << "  Thank you for playing!        \n";
    std::cout << "=================================\n";

    return true;
}

bool ConsoleRenderer::drawSettings() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "           SETTINGS             \n";
    std::cout << "=================================\n\n";
    std::cout << "   Control Settings:\n\n";
    std::cout << "   Move Up: W / Up Arrow\n";
    std::cout << "   Move Down: S / Down Arrow\n";
    std::cout << "   Move Left: A / Left Arrow\n";
    std::cout << "   Move Right: D / Right Arrow\n";
    std::cout << "   Fire: SPACE / F\n";
    std::cout << "   Pause: P\n";
    std::cout << "   Menu: M\n\n";
    std::cout << "   Game Settings:\n\n";

    return true;
}

bool ConsoleRenderer::drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "         MAP SELECTION          \n";
    std::cout << "=================================\n\n";
    std::cout << "   Current Map: " << currentMap.displayName << "\n";
    std::cout << "   " << currentMap.description << "\n";
    std::cout << "   Map " << (currentIndex + 1) << " of " << totalMaps << "\n\n";
    std::cout << "---------------------------------\n";
    std::cout << "         MAP PREVIEW            \n";
    std::cout << "---------------------------------\n";
    
    // Рисуем превью карты
    drawMapPreview(currentMap);
    
    std::cout << "\n---------------------------------\n";
    std::cout << "          CONTROLS              \n";
    std::cout << "---------------------------------\n\n";
    std::cout << "   [A] - Previous Map\n";
    std::cout << "   [D] - Next Map\n";
    std::cout << "   [F] - Random Generation\n";
    std::cout << "   [ENTER] - Start Game\n";
    std::cout << "   [M] - Main Menu\n";
    std::cout << "   [Q] - Exit\n\n";
    std::cout << "=================================\n";

    return true;
}

void ConsoleRenderer::drawMapPreview(const MapInfo& map) {
    bool useUnicode = PlatformUtils::supportsUnicode();
    auto graphicsMap = getGraphicsMap(useUnicode, useAdvancedGraphics);
    
    for (int y = 0; y < map.height; y++) {
        std::cout << "   ";
        
        for (int x = 0; x < map.width; x++) {
            char symbol = map.layout[y][x];
            
            // Используем графическую карту для улучшенного отображения
            auto it = graphicsMap.find(symbol);
            if (it != graphicsMap.end()) {
                setColor(it->second.second);
                std::cout << it->second.first;
                resetColor();
            } else {
                // Для неизвестных символов используем стандартное отображение
                if (symbol == 'E') {
                    setColor(PlatformUtils::Color::RED);
                    std::cout << "E"; // Враги
                    resetColor();
                } else if (symbol == '^' || symbol == 'v' || symbol == '<' || symbol == '>') {
                    setColor(PlatformUtils::Color::GREEN);
                    std::cout << "P"; // Игрок
                    resetColor();
                } else {
                    std::cout << symbol;
                }
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    // Статистика карты
    std::cout << "   Size: " << map.width << "x" << map.height;
    
    // Подсчитываем объекты
    int walls = 0, water = 0, forest = 0, enemies = 0, empty = 0;
    for (const auto& row : map.layout) {
        for (char cell : row) {
            if (cell == '#' || cell == 'X') walls++;
            else if (cell == '~') water++;
            else if (cell == '*') forest++;
            else if (cell == 'E') enemies++;
            else if (cell == ' ') empty++;
        }
    }
    
    std::cout << " | Empty cells: " << empty << "\n";
    std::cout << "   Objects: " << walls << " walls, " << water << " water, " << forest << " forest, " << enemies << " enemies\n";
}

bool ConsoleRenderer::drawLevelComplete(int score, int level, int lives) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "         LEVEL " << level << " COMPLETED!     \n";
    std::cout << "=================================\n\n";
    std::cout << "        LEVEL STATISTICS        \n";
    std::cout << "---------------------------------\n\n";
    std::cout << "   Points earned: " << score << "\n";
    std::cout << "   Lives remaining: " << lives << "\n\n";
    
    // Бонусы за прохождение уровня
    int levelBonus = level * 100;
    int livesBonus = lives * 25;
    int totalBonus = levelBonus + livesBonus;
    
    std::cout << "        LEVEL BONUSES           \n";
    std::cout << "---------------------------------\n\n";
    std::cout << "   For level " << level << ": " << levelBonus << " points\n";
    std::cout << "   For remaining lives: " << livesBonus << " points\n";
    std::cout << "   TOTAL BONUS: " << totalBonus << " points\n\n";
    std::cout << "   NEW SCORE: " << (score + totalBonus) << " points\n\n";
    std::cout << "---------------------------------\n";
    std::cout << "   [ENTER] - Next Level\n";
    std::cout << "   [M] - Main Menu\n";
    std::cout << "   [Q] - Exit\n";
    std::cout << "---------------------------------\n\n";
    std::cout << "=================================\n";
    std::cout << "  Prepare for level " << (level + 1) << "!\n";
    std::cout << "=================================\n";

    return true;
}
