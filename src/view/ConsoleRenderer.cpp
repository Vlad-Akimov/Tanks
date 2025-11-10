#include "ConsoleRenderer.h"
#include <iostream>
#include <string>
#include <vector>

ConsoleRenderer::ConsoleRenderer(int width, int height) 
    : screenWidth(width), screenHeight(height), terminalSizeValid(true) {
    updateTerminalSize();
}

void ConsoleRenderer::updateTerminalSize() {
    auto size = PlatformUtils::getTerminalSize();
    terminalWidth = size.first;
    terminalHeight = size.second;
    
    int requiredWidth = screenWidth;
    int requiredHeight = screenHeight;
    
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

void ConsoleRenderer::drawErrorMessage(const std::string& message) {
    clearScreen();
    
    std::cout << "\n\n";
    std::cout << "============================================\n";
    drawCenteredText("ОШИБКА ОТОБРАЖЕНИЯ", terminalWidth);
    std::cout << "\n";
    std::cout << "============================================\n\n";
    
    drawCenteredText(message, terminalWidth);
    std::cout << "\n\n";
    
    std::cout << "Текущий размер: " << terminalWidth << "x" << terminalHeight << "\n";
    std::cout << "Требуемый размер: " << (screenWidth + 4) << "x" << (screenHeight + 10) << "\n\n";
    
    std::cout << "============================================\n";
    drawCenteredText("Увеличьте размер окна и перезапустите игру", terminalWidth);
    std::cout << "\n";
    std::cout << "============================================\n";
}

void ConsoleRenderer::render(const GameWorld& world) {
    if (!checkTerminalSize()) {
        drawErrorMessage("Размер терминала слишком мал для отображения игры");
        return;
    }

    bool damageFlashActive = world.isDamageFlashActive();

    // Рисуем верхнюю границу с информацией
    std::cout << "Уровень: " << world.getCurrentLevel() 
              << " | Очки: " << world.getPlayer()->getScore()
              << " | Жизни: " << world.getPlayer()->getLives()
              << " | Здоровье: " << world.getPlayer()->getHealth() << "\n";
    
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    drawBorder();
    if (damageFlashActive) {
        resetColor();
    }
    
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

    for (const auto& explosion : world.getExplosions()) {
        if (explosion->isDestroyed()) continue;
        
        Point pos = explosion->getPosition();
        if (pos.x >= 0 && pos.x < screenWidth && 
            pos.y >= 0 && pos.y < screenHeight) {
            buffer[pos.y][pos.x] = explosion->getSymbol();
        }
    }
    
    // Выводим буфер на экран с цветами
    for (int y = 0; y < screenHeight; y++) {
        if (damageFlashActive) {
            setColor(PlatformUtils::Color::RED);
        }
        std::cout << "|"; // Левая граница
        if (damageFlashActive) {
            resetColor();
        }
        for (int x = 0; x < screenWidth; x++) { 
            char symbol = buffer[y][x];
            
            // Определяем цвет для символа
            bool colored = false;
            
            // Проверяем, является ли символ танком игрока
            if (symbol == '^' || symbol == 'v' || symbol == '<' || symbol == '>') {
                setColor(PlatformUtils::Color::GREEN);
                colored = true;
            }
            // Проверяем, является ли символ вражеским танком (включая новые типы)
            else if (symbol == 'A' || symbol == 'V' || symbol == '[' || symbol == ']' || 
                     symbol == 'E' || symbol == 'F' || symbol == 'D') {
                setColor(PlatformUtils::Color::RED);
                colored = true;
            }
            // Проверяем, является ли символ бонусом
            else if (symbol == 'S' || symbol == 'K' || symbol == 'B' || symbol == 'L') {
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
            else if (symbol == '~') {
                setColor(PlatformUtils::Color::BLUE);
                colored = true;
            }
            else if (symbol == '*') {
                setColor(PlatformUtils::Color::GREEN);
                colored = true;
            }

            else if (symbol == 'O') {
                setColor(PlatformUtils::Color::RED);
                colored = true;
            }
            
            std::cout << symbol;
            
            // Сбрасываем цвет после вывода символа
            if (colored) {
                resetColor();
            }
        }

        if (damageFlashActive) {
            setColor(PlatformUtils::Color::RED);
        }
        std::cout << "|\n"; // Правая граница
        if (damageFlashActive) {
            resetColor();
        }
    }
    
    if (damageFlashActive) {
        setColor(PlatformUtils::Color::RED);
    }
    drawBorder();
    if (damageFlashActive) {
        resetColor();
    }
    
    // Отображаем статус бонусов игрока
    PlayerTank* player = world.getPlayer();
    if (player) {
        std::cout << "Бонусы: ";
        if (player->getHasShield()) { std::cout << "[Щит] "; }
        if (player->getDoubleFire()) { std::cout << "[Двойной огонь] "; }
        if (player->getBonusDuration() > 0) { std::cout << "(осталось ходов: " << player->getBonusDuration() << ")"; }
        std::cout << "\n";
    }
    
    drawSymbolLegend();
}

void ConsoleRenderer::drawSymbolLegend() {
    std::cout << "\n=== ЛЕГЕНДА СИМВОЛОВ ===\n";
    
    // Танки
    std::cout << "Танки: ";
    setColor(PlatformUtils::Color::GREEN);
    std::cout << "^ v < >";
    resetColor();
    std::cout << " - игрок\n";
    
    std::cout << "Враги: ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "E";
    resetColor();
    std::cout << " - обычный, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "F";
    resetColor();
    std::cout << " - быстрый, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "D";
    resetColor();
    std::cout << " - сильный, ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "A";
    resetColor();
    std::cout << " - бронированный\n";
    
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
    setColor(PlatformUtils::Color::GREEN);
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
    std::cout << "K";
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

    std::cout << "Взрывы: ";
    setColor(PlatformUtils::Color::RED);
    std::cout << "O";
    resetColor();
    std::cout << " - взрыв (1 ход)\n";
}

void ConsoleRenderer::drawMenu() {
    if (!checkTerminalSize()) {
        drawErrorMessage("Размер терминала слишком мал для отображения меню");
        return;
    }

    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "        ТАНКОВЫЙ СИМУЛЯТОР       \n";
    std::cout << "=================================\n\n";
    
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
    
    std::cout << "   Размер поля: " << screenWidth << "x" << screenHeight << "\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "   [ESC] - Назад\n";
    std::cout << "   [Q] - Выход\n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "=================================\n";
    std::cout << "  Настройки сохраняются автоматически\n";
    std::cout << "=================================\n";
}

void ConsoleRenderer::drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps) {
    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "        ВЫБОР КАРТЫ             \n";
    std::cout << "=================================\n\n";
    
    std::cout << "   Текущая карта: " << currentMap.displayName << "\n";
    std::cout << "   " << currentMap.description << "\n";
    std::cout << "   Карта " << (currentIndex + 1) << " из " << totalMaps << "\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "        ПРЕВЬЮ КАРТЫ            \n";
    std::cout << "---------------------------------\n";
    
    // Рисуем превью карты
    drawMapPreview(currentMap);
    
    std::cout << "\n---------------------------------\n";
    std::cout << "          УПРАВЛЕНИЕ           \n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "   [A] - Предыдущая карта\n";
    std::cout << "   [D] - Следующая карта\n";
    std::cout << "   [F] - Случайная генерация\n";
    std::cout << "   [ENTER] - Начать игру\n";
    std::cout << "   [M] - Главное меню\n";
    std::cout << "   [Q] - Выход\n\n";
    
    std::cout << "=================================\n";
}

void ConsoleRenderer::drawMapPreview(const MapInfo& map) {
    for (int y = 0; y < map.height; y++) {
        std::cout << "   ";
        
        for (int x = 0; x < map.width; x++) {
            char symbol = map.layout[y][x];
            
            // Определяем цвет для символа
            if (symbol == '#' || symbol == 'X') {
                std::cout << symbol; // Стены
            } else if (symbol == '~') {
                setColor(PlatformUtils::Color::BLUE);
                std::cout << symbol; // Вода
                resetColor();
            } else if (symbol == '*') {
                setColor(PlatformUtils::Color::GREEN);
                std::cout << symbol; // Лес
                resetColor();
            } else if (symbol == 'E') {
                setColor(PlatformUtils::Color::RED);
                std::cout << symbol; // Враги
                resetColor();
            } else if (symbol == '^' || symbol == 'v' || symbol == '<' || symbol == '>') {
                setColor(PlatformUtils::Color::GREEN);
                std::cout << "P"; // Игрок
                resetColor();
            } else {
                std::cout << " "; // Пустое пространство
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    // Статистика карты
    std::cout << "   Размер: " << map.width << "x" << map.height;
    
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
    
    std::cout << " | Пустых клеток: " << empty << "\n";
    std::cout << "   Объекты: " << walls << " стен, " << water << " воды, " 
              << forest << " леса, " << enemies << " врагов\n";
}

void ConsoleRenderer::drawLevelComplete(int score, int level, int lives) {
    clearScreen();
    
    std::cout << "=================================\n";
    std::cout << "         УРОВЕНЬ " << level << " ПРОЙДЕН!      \n";
    std::cout << "=================================\n\n";
    
    std::cout << "        СТАТИСТИКА УРОВНЯ       \n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "   Очков заработано: " << score << "\n";
    std::cout << "   Осталось жизней: " << lives << "\n\n";
    
    // Бонусы за прохождение уровня
    int levelBonus = level * 100;
    int livesBonus = lives * 25;
    int totalBonus = levelBonus + livesBonus;
    
    std::cout << "        БОНУСЫ ЗА УРОВЕНЬ       \n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "   За уровень " << level << ": " << levelBonus << " очков\n";
    std::cout << "   За оставшиеся жизни: " << livesBonus << " очков\n";
    std::cout << "   ОБЩИЙ БОНУС: " << totalBonus << " очков\n\n";
    
    std::cout << "   НОВЫЙ СЧЕТ: " << (score + totalBonus) << " очков\n\n";
    
    std::cout << "---------------------------------\n";
    std::cout << "   [ENTER] - Следующий уровень\n";
    std::cout << "   [M] - Главное меню\n";
    std::cout << "   [Q] - Выход\n";
    std::cout << "---------------------------------\n\n";
    
    std::cout << "=================================\n";
    std::cout << "  Готовьтесь к уровню " << (level + 1) << "!\n";
    std::cout << "=================================\n";
}
