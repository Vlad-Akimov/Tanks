#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../model/GameWorld.h"
#include "../view/ConsoleRenderer.h"
#include "InputHandler.h"

class GameController {
private:
    GameWorld model;
    ConsoleRenderer view;
    InputHandler inputHandler;
    bool running;

public:
    GameController(int width, int height);
    
    // Основной цикл игры
    void runGame();
    void pauseGame();
    void saveSettings();
    
private:
    // Вспомогательные методы
    void handleInput();
    void processGameLogic();
    void processCommand(Command cmd);
    void showMenu();
    void showSettings();
};

#endif // GAMECONTROLLER_H