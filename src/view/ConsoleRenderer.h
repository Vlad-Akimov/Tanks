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
    void drawPixel(char symbol, PlatformUtils::Color color);

    void setColor(PlatformUtils::Color color);
    void resetColor();
    void drawBorder();
    void drawMapPreview(const MapInfo& map);

    bool useAdvancedGraphics;

public:
    ConsoleRenderer();
    bool checkTerminalSize();
    void drawErrorMessage(const std::string& message);
    void clearScreen();
    void setCursorPosition(int x, int y);
    bool render(const GameWorld& world);
    void drawSymbolLegend();
    bool drawMenu();
    bool drawPauseScreen();
    bool drawGameOver(int score);
    bool drawSettings();
    bool drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps);
    bool drawLevelComplete(int score, int level, int lives);

    void setAdvancedGraphics(bool enabled);
    bool getAdvancedGraphics() const;

    bool isTerminalSizeValid() const;
};

#endif // CONSOLERENDERER_H
