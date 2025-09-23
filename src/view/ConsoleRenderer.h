#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "../model/GameWorld.h"

class ConsoleRenderer {
private:
    int screenWidth, screenHeight;

public:
    ConsoleRenderer(int width, int height);
    
    // Основные методы отрисовки
    void render(const GameWorld& world);
    void drawMenu();
    void drawPauseScreen();
    void drawGameOver(int score);
    void drawSettings();
    
    // Вспомогательные методы
    void clearScreen();
    void setCursorPosition(int x, int y);
};

#endif // CONSOLERENDERER_H