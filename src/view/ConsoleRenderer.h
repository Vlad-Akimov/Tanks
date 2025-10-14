#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "../model/GameWorld.h"
#include "../utils/PlatformUtils.h"
#include <iostream>
#include <string>
#include <vector>

class ConsoleRenderer {
private:
    int screenWidth, screenHeight;
    
    void setColor(PlatformUtils::Color color);
    void resetColor();
    void drawBorder();

public:
    ConsoleRenderer(int width, int height);
    void clearScreen();
    void setCursorPosition(int x, int y);
    void render(const GameWorld& world);
    void drawSymbolLegend();
    void drawMenu();
    void drawPauseScreen();
    void drawGameOver(int score);
    void drawSettings();
};

#endif // CONSOLERENDERER_H
