#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "../model/GameWorld.h"
#include "../model/MapManager.h"
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
    void drawMapPreview(const MapInfo& map, int currentIndex, int totalMaps);

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
    void drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps);
};

#endif // CONSOLERENDERER_H
