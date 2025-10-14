#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../model/GameWorld.h"
#include "../model/MapManager.h"
#include "../view/ConsoleRenderer.h"
#include "../utils/PlatformUtils.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>

enum class Command { 
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, 
    FIRE, PAUSE, MENU, CONFIRM, BACK, EXIT, NONE
};

class InputHandler {
private:
    std::map<int, Command> keyBindings;
    std::map<std::string, Command> utf8KeyBindings;

public:
    InputHandler();
    Command waitForCommand();
    void remapKey(int keyCode, Command command);
    void remapUTF8Key(const std::string& utf8Char, Command command);
    static int getKeyCode(char c);
};

class GameController {
private:
    GameWorld model;
    ConsoleRenderer view;
    InputHandler inputHandler;
    bool running;

    MapManager mapManager;
    int currentMapIndex;

    void processGameTurn();
    void processCommand(Command cmd);
    void showMenu();
    void showSettings();

    void showMapSelection();
    void loadSelectedMap();

public:
    GameController(int width, int height);
    void runGame();
    void pauseGame();
    void saveSettings();
};

#endif // GAMECONTROLLER_H
