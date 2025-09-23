// src/controller/GameController.h
#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../model/GameModel.h"
#include "../view/GameView.h"
#include "InputHandler.h"
#include "../utils/Timer.h"

class GameController {
private:
    GameModel model;
    GameView view;
    InputHandler inputHandler;
    Timer gameTimer;
    bool isRunning;
    int targetFPS;

public:
    GameController();
    ~GameController();
    
    void run();
    void pauseGame();
    void resumeGame();
    void exitGame();
    void handleInput();
    void processGameLogic();
    void updateView();
    
    void saveSettings();
    void loadSettings();
    
    // Menu navigation
    void showMainMenu();
    void showSettingsMenu();
    void startNewGame();
    void continueGame();
};

#endif