#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../controller/InputHandler.h"
#include "../model/GameWorld.h"
#include "../utils/MapManager.h"
#include "../view/ConsoleRenderer.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>

class GameController {
private:
    GameWorld model;
    ConsoleRenderer view;
    InputHandler inputHandler;
    bool running;

    MapManager mapManager;
    int currentMapIndex;
    bool useCustomMap;
    MapInfo selectedMap;

    void processGameTurn();
    void processCommand(Command cmd);
    void showMenu();
    void showSettings();

    void showMapSelection();
    void loadSelectedMap();
    void loadNextLevel();

public:
    GameController(int width, int height);
    void runGame();
    void pauseGame();
    void saveSettings();
};

#endif // GAMECONTROLLER_H
