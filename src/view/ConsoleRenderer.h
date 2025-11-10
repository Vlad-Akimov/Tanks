#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "../model/GameWorld.h"
#include "../utils/MapManager.h"
#include "../utils/PlatformUtils.h"
#include <iostream>
#include <string>
#include <vector>

class ConsoleRenderer {
private:
    int screenWidth, screenHeight;
    int terminalWidth, terminalHeight;
    bool terminalSizeValid;
    
    void updateTerminalSize();
    void drawCenteredText(const std::string& text, int width);
    

    void setColor(PlatformUtils::Color color);
    void resetColor();
    void drawBorder();
    void drawMapPreview(const MapInfo& map);

public:
    ConsoleRenderer(int width, int height);
    bool checkTerminalSize();
    void drawErrorMessage(const std::string& message);
    void clearScreen();
    void setCursorPosition(int x, int y);
    void render(const GameWorld& world);
    void drawSymbolLegend();
    void drawMenu();
    void drawPauseScreen();
    void drawGameOver(int score);
    void drawSettings();
    void drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps);
    void drawLevelComplete(int score, int level, int lives);
};

#endif // CONSOLERENDERER_H
