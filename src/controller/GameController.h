/**
 * @file GameController.h
 * @author Vld251
 * @brief Central game controller coordinating model, view, and input systems.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "../controller/InputHandler.h"
#include "../model/GameWorld.h"
#include "../utils/MapManager.h"
#include "../utils/ScoreManager.h"
#include "../view/ConsoleRenderer.h"
#include "../utils/SettingsManager.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>

/**
 * @brief Main game controller class that manages the game flow and logic.
 * 
 * Acts as the central coordinator between model (GameWorld), view (ConsoleRenderer),
 * and input handling. Manages game states, menus, level progression, and user interactions.
 */
class GameController {
private:
    GameWorld model;               ///< Game model containing world state and logic
    ConsoleRenderer view;          ///< View component for rendering
    InputHandler inputHandler;     ///< Input handler for user commands
    ScoreManager scoreManager;     ///< Manager for high scores
    bool running;                  ///< Flag indicating if game is running
    MapManager mapManager;         ///< Manager for game maps
    int currentMapIndex;           ///< Index of currently selected map
    bool scoreSaved;               ///< Flag indicating if score has been saved
    bool useCustomMap;             ///< Flag indicating if custom map is being used
    
    MapInfo selectedMap;           ///< Currently selected map information

    /**
     * @brief Processes a single game turn/update cycle.
     * @returns None
     */
    void processGameTurn();
    
    /**
     * @brief Processes a user command.
     * @param cmd Command to process.
     * @returns None
     */
    void processCommand(Command cmd);
    
    /**
     * @brief Displays and manages the main menu.
     * @returns None
     */
    void showMenu();
    
    /**
     * @brief Displays and manages the settings menu.
     * @returns None
     */
    void showSettings();
    
    /**
     * @brief Displays level completion screen.
     * @returns None
     */
    void showLevelCompleteScreen();

    /**
     * @brief Displays map selection screen.
     * @returns None
     */
    void showMapSelection();
    
    /**
     * @brief Loads the currently selected map.
     * @returns None
     */
    void loadSelectedMap();
    
    /**
     * @brief Loads the next game level.
     * @returns None
     */
    void loadNextLevel();

    SettingsManager settingsManager;  ///< Manager for game settings
    
    /**
     * @brief Toggles between advanced and symbolic graphics modes.
     * @returns None
     */
    void toggleAdvancedGraphics();

    /**
     * @brief Handles terminal resize events.
     * @returns None
     */
    void handleTerminalResize();

public:
    /**
     * @brief Constructs a GameController object.
     * @param width Width of game world.
     * @param height Height of game world.
     * @returns None
     */
    GameController(int width, int height);
    
    /**
     * @brief Starts and runs the main game loop.
     * @returns None
     */
    void runGame();
    
    /**
     * @brief Pauses the game and displays pause screen.
     * @returns None
     */
    void pauseGame();
    
    /**
     * @brief Saves current game settings.
     * @returns None
     */
    void saveSettings();
};

#endif // GAMECONTROLLER_H
