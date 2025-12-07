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

int ConsoleRenderer::calculateHorizontalOffset(int contentWidth) const {
    if (contentWidth >= terminalWidth) {
        return 0;
    }
    return (terminalWidth - contentWidth) / 2;
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
    std::cout << "██";
    for (int i = 0; i < screenWidth; i++) { std::cout << "█"; }
    std::cout << "██\n";
}

void ConsoleRenderer::clearScreen() {
    PlatformUtils::clearScreen();
}

void ConsoleRenderer::setCursorPosition(int x, int y) {
    PlatformUtils::setCursorPosition(x, y);
}

void ConsoleRenderer::drawErrorMessage(const std::string& message) {
    clearScreen();
    
    std::vector<std::string> errorLines = {
        "============================================",
        "DISPLAY ERROR",
        "============================================",
        "",
        message,
        "",
        "Current size: " + std::to_string(terminalWidth) + "x" + std::to_string(terminalHeight),
        "Required size: " + std::to_string(60) + "x" + std::to_string(40),
        "",
        "============================================",
        "Increase window size",
        "============================================"
    };
    
    // Выводим все строки с центрированием
    for (const auto& line : errorLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
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

    // Рассчитываем отступ для центрирования игрового поля
    int gameFieldWidth = screenWidth + 4; // +4 для границ (██ с двух сторон)
    int horizontalOffset = calculateHorizontalOffset(gameFieldWidth);

    // Очищаем экран
    clearScreen();

    // Выводим верхнюю информацию о статусе игры
    std::string statusLine = "Level: " + std::to_string(world.getCurrentLevel()) 
                           + " | Score: " + std::to_string(world.getPlayer()->getScore())
                           + " | Lives: " + std::to_string(world.getPlayer()->getLives())
                           + " | Health: " + std::to_string(world.getPlayer()->getHealth());
    
    // Центрируем строку статуса
    int statusOffset = calculateHorizontalOffset(statusLine.length());
    std::cout << std::string(statusOffset, ' ') << statusLine << "\n\n";
    
    // Рисуем верхнюю границу с отступом
    setColor(PlatformUtils::Color::BLACK);
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    std::cout << std::string(horizontalOffset, ' ');
    drawBorder();
    resetColor();
    
    // Создаем буфер для отрисовки
    std::vector<std::vector<char>> buffer(screenHeight, std::vector<char>(screenWidth, ' '));
    
    // Заполняем буфер объектами (существующий код без изменений)
    for (const auto& obj : world.getObjects()) {
        if (obj->isDestroyed()) continue;
        
        Point pos = obj->getPosition();
        Point bounds = obj->getBounds();
        char symbol = obj->getSymbol();
        
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
    
    // Выводим буфер на экран с центрированием
    for (int y = 0; y < screenHeight; y++) {
        // Добавляем отступ для центрирования
        std::cout << std::string(horizontalOffset, ' ');
        
        setColor(PlatformUtils::Color::BLACK);
        if (damageFlashActive) {
            setColor(PlatformUtils::Color::RED);
        }
        std::cout << "██"; // Левая граница
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
        std::cout << "██\n"; // Правая граница
        resetColor();
    }
    
    // Рисуем нижнюю границу с отступом
    setColor(PlatformUtils::Color::BLACK);
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    std::cout << std::string(horizontalOffset, ' ');
    drawBorder();
    resetColor();
    
    // Отображаем статус бонусов игрока
    PlayerTank* player = world.getPlayer();
    if (player) {
        std::string bonusLine = "Bonuses: ";
        if (player->getHasShield()) { 
            bonusLine += "[★ Shield] ";
        }
        if (player->getDoubleFire()) { 
            bonusLine += "[¶ Double Fire] ";
        }
        if (player->getBonusDuration() > 0) { 
            bonusLine += "(turns left: " + std::to_string(player->getBonusDuration()) + ")";
        }
        
        // Центрируем строку бонусов
        int bonusOffset = calculateHorizontalOffset(bonusLine.length());
        std::cout << std::string(bonusOffset, ' ') << bonusLine << "\n";
    }
    
    drawSymbolLegend();

    return true;
}

void ConsoleRenderer::drawSymbolLegend() {
    bool useUnicode = PlatformUtils::supportsUnicode();
    auto graphicsMap = getGraphicsMap(useUnicode, useAdvancedGraphics);
    
    // Создаем вектор строк для легенды
    std::vector<std::string> legendLines = {
        "=== SYMBOL LEGEND ===",
        "",
        "Tanks:",
        "",
        "  Player: " + graphicsMap.find('^')->second.first + " " + 
                    graphicsMap.find('v')->second.first + " " + 
                    graphicsMap.find('<')->second.first + " " + 
                    graphicsMap.find('>')->second.first,
        "",
        "  Enemies:  " + graphicsMap.find('E')->second.first + " - normal" + "  " + graphicsMap.find('F')->second.first + " - fast" + "  " + graphicsMap.find('D')->second.first + " - strong" + "  " + graphicsMap.find('A')->second.first + " - armored",
        "",
        "Obstacles:  " + graphicsMap.find('#')->second.first + " - brick" + "  " + graphicsMap.find('X')->second.first + " - steel" + "  " + graphicsMap.find('~')->second.first + " - water" + "  " + graphicsMap.find('*')->second.first + " - forest",
        "",
        "Bonuses:  " + graphicsMap.find('S')->second.first + " - shield" + "  " + graphicsMap.find('K')->second.first + " - double fire" + "  " + graphicsMap.find('B')->second.first + " - speed" + "  " + graphicsMap.find('L')->second.first + " - +1 life",
        "",
        "Explosions:    " + graphicsMap.find('O')->second.first,
        ""
    };
    
    // Выводим каждую строку легенды по центру
    for (const auto& line : legendLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
}

bool ConsoleRenderer::drawMenu() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::vector<std::string> menuLines = {
        "=================================",
        "TANKS GAME",
        "=================================",
        "",
        "---------------------------------",
        "MAIN MENU",
        "---------------------------------",
        "",
        "[ENTER] - Start Game",
        "[M] - Settings",
        "[Q] - Exit",
        "",
        "---------------------------------",
        "IN-GAME CONTROLS",
        "---------------------------------",
        "",
        "Movement: WASD or arrow keys",
        "Fire: SPACE or F",
        "Pause: P",
        "Menu: M",
        "Exit: Q",
        ""
    };
    
    for (const auto& line : menuLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
    
    return true;
}

bool ConsoleRenderer::drawPauseScreen() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::vector<std::string> pauseLines = {
        "=================================",
        "PAUSED",
        "=================================",
        "",
        " Game is paused",
        "",
        "---------------------------------",
        "[P] or [ENTER] - Continue",
        "[M] - Main Menu",
        "[Q] - Exit",
        "---------------------------------",
        "",
        "================================="
    };
    
    for (const auto& line : pauseLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
    
    return true;
}

bool ConsoleRenderer::drawGameOver(int score) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    std::vector<std::string> gameOverLines = {
        "=================================",
        "GAME OVER",
        "=================================",
        "",
        " Your score: " + std::to_string(score),
        "",
        "---------------------------------",
        "ENTER] - New Game",
        "[M] - Main Menu",
        "[Q] - Exit",
        "---------------------------------",
        "",
        "=================================",
        "Thank you for playing!",
        "================================="
    };
    
    for (const auto& line : gameOverLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
    
    return true;
}

bool ConsoleRenderer::drawSettings() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    bool advancedGraphics = PlatformUtils::supportsUnicode() && useAdvancedGraphics;
    
    std::vector<std::string> settingsLines = {
        "=================================",
        "SETTINGS",
        "=================================",
        "",
        "Control Settings:",
        "",
        "Move Up: W / Up Arrow",
        "Move Down: S / Down Arrow",
        "Move Left: A / Left Arrow",
        "Move Right: D / Right Arrow",
        "Fire: SPACE / F",
        "Pause: P",
        "Menu: M",
        "",
        "Game Settings:",
        "",
        "Graphics: [" + std::string(advancedGraphics ? "X" : " ") + "] Advanced",
        "          [" + std::string(!advancedGraphics ? "X" : " ") + "] Symbolic",
        "",
        "[F] - Toggle graphics mode",
        "---------------------------------",
        "Status: " + std::string(advancedGraphics ? 
            "Advanced graphics (Unicode)" : 
            "Symbolic graphics (ASCII)"),
        "",
        "[ESC] - Back",
        "[Q] - Exit",
        "---------------------------------",
        "",
        "=================================",
        "Settings are saved automatically",
        "================================="
    };
    
    for (const auto& line : settingsLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }

    return true;
}

bool ConsoleRenderer::drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    // Сначала выводим текст меню
    std::vector<std::string> mapSelectionLines = {
        "=================================",
        "MAP SELECTION",
        "=================================",
        "",
        " Current Map: " + currentMap.displayName,
        "   " + currentMap.description,
        " Map " + std::to_string(currentIndex + 1) + " of " + std::to_string(totalMaps),
        "",
        "---------------------------------",
        "MAP PREVIEW",
        "---------------------------------"
    };
    
    for (const auto& line : mapSelectionLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
    
    // Рисуем превью карты с центрированием
    drawMapPreview(currentMap);
    
    // Выводим нижнюю часть меню
    std::vector<std::string> controlsLines = {
        "",
        "[A] - Previous Map",
        "[D] - Next Map",
        "[F] - Random Generation",
        "[ENTER] - Start Game",
        "[M] - Main Menu",
        "[Q] - Exit",
        "",
        "================================="
    };
    
    for (const auto& line : controlsLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }

    return true;
}

void ConsoleRenderer::drawMapPreview(const MapInfo& map) {
    bool useUnicode = PlatformUtils::supportsUnicode();
    auto graphicsMap = getGraphicsMap(useUnicode, useAdvancedGraphics);
    
    // Рассчитываем отступ для центрирования превью карты
    int previewWidth = map.width + 4; // +4 для отступов и границ
    int offset = calculateHorizontalOffset(previewWidth);
    
    for (int y = 0; y < map.height; y++) {
        std::cout << std::string(offset, ' ') << "  ";
        
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
}

bool ConsoleRenderer::drawLevelComplete(int score, int level, int lives) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Terminal size too small. Increase window size.");
        return false;
    }

    clearScreen();
    
    // Бонусы за прохождение уровня
    int levelBonus = level * 100;
    int livesBonus = lives * 25;
    int totalBonus = levelBonus + livesBonus;
    int newScore = score + totalBonus;
    
    std::vector<std::string> levelCompleteLines = {
        "=================================",
        "LEVEL " + std::to_string(level) + " COMPLETED!",
        "=================================",
        "",
        "LEVEL STATISTICS",
        "---------------------------------",
        "",
        "   Points earned: " + std::to_string(score),
        "   Lives remaining: " + std::to_string(lives),
        "",
        "LEVEL BONUSES",
        "---------------------------------",
        "",
        "For level " + std::to_string(level) + ": " + std::to_string(levelBonus) + " points",
        "For remaining lives: " + std::to_string(livesBonus) + " points",
        "TOTAL BONUS: " + std::to_string(totalBonus) + " points",
        "",
        "NEW SCORE: " + std::to_string(newScore) + " points",
        "",
        "---------------------------------",
        "[ENTER] - Next Level",
        "[M] - Main Menu",
        "[Q] - Exit",
        "---------------------------------",
        "",
        "=================================",
        "Prepare for level " + std::to_string(level + 1) + "!",
        "================================="
    };
    
    for (const auto& line : levelCompleteLines) {
        int offset = calculateHorizontalOffset(line.length());
        std::cout << std::string(offset, ' ') << line << "\n";
    }
    
    return true;
}
