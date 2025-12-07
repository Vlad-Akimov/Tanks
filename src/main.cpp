/**
 * @file main.cpp
 * @author Vld251
 * @brief 
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include "controller/GameController.h"
#include <algorithm>

/**
 * @brief Main entry point for the Tank Simulator game.
 * 
 * Initializes the game environment, checks system requirements,
 * creates the game controller, and starts the game loop.
 * 
 * @file main.cpp
 * @brief Main application entry point
 */

/**
 * @brief Main function that serves as the application entry point.
 * 
 * This function performs the following tasks:
 * 1. Checks terminal size requirements
 * 2. Initializes game dimensions based on terminal size
 * 3. Creates and runs the GameController
 * 4. Handles exceptions and error conditions
 * 
 * @return int - Exit status code:
 *               - 0: Successful execution
 *               - -1: Error during execution
 */
int main() {
    try {
        // Minimum terminal dimensions required for proper game display
        const int MIN_WIDTH = 60;    ///< Minimum terminal width in characters
        const int MIN_HEIGHT = 40;   ///< Minimum terminal height in characters

        // Check if terminal meets minimum size requirements
        if (!PlatformUtils::isTerminalSizeValid(MIN_WIDTH, MIN_HEIGHT)) {
            // Display resize message and exit if terminal is too small
            PlatformUtils::showResizeMessage(MIN_WIDTH, MIN_HEIGHT);
            return -1;
        }

        // Get current terminal dimensions
        auto [termWidth, termHeight] = PlatformUtils::getTerminalSize();

        // Calculate game dimensions based on terminal size
        // Use minimum of required and available dimensions
        int gameWidth = std::min(MIN_WIDTH, termWidth);
        int gameHeight = std::min(MIN_HEIGHT, termHeight);
        
        // Create and initialize the main game controller
        GameController game(gameWidth, gameHeight);
        
        // Start the main game loop
        game.runGame();
    } catch (const std::exception& e) {
        // Handle any exceptions that occur during execution
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
    
    // Successful execution
    return 0;
}
