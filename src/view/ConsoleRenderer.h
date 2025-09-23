#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "..\model\GameWorld.h"

class ConsoleRenderer {
private:
    static const int WIDTH = 40;
    static const int HEIGHT = 20;
    char buffer[HEIGHT][WIDTH];

public:
    ConsoleRenderer();
    
    // Методы отрисовки
    void clearBuffer();
    void drawGameWorld(const GameWorld& world);
    void drawMenu();
    void drawPauseScreen();
    void drawGameOverScreen(bool win);
    void render();
};

#endif // CONSOLERENDERER_H