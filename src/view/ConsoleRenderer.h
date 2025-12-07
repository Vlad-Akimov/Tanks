/**
 * @file ConsoleRenderer.h
 * @author Vld251
 * @brief Renders game state, menus, and UI elements to the console.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include "../model/GameWorld.h"
#include "../utils/MapManager.h"
#include "../utils/PlatformUtils.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Class responsible for console-based rendering of the game.
 * 
 * Handles all display operations including game field rendering, menus,
 * UI elements, and visual effects. Supports both ASCII and Unicode graphics.
 */
class ConsoleRenderer {
private:
    int screenWidth;          ///< Width of game screen in characters
    int screenHeight;         ///< Height of game screen in characters
    int terminalWidth;        ///< Actual terminal width
    int terminalHeight;       ///< Actual terminal height
    bool terminalSizeValid;   ///< Whether terminal meets minimum size requirements
    
    /**
     * @brief Updates terminal size information.
     * @returns None
     */
    void updateTerminalSize();
    
    /**
     * @brief Draws centered text within specified width.
     * @param text Text to center.
     * @param width Width to center within.
     * @returns None
     */
    void drawCenteredText(const std::string& text, int width);
    
    /**
     * @brief Draws a single colored pixel/character.
     * @param symbol Character to draw.
     * @param color Color to use.
     * @returns None
     */
    void drawPixel(char symbol, PlatformUtils::Color color);

    /**
     * @brief Sets terminal text color.
     * @param color Color to set.
     * @returns None
     */
    void setColor(PlatformUtils::Color color);
    
    /**
     * @brief Resets terminal color to default.
     * @returns None
     */
    void resetColor();
    
    /**
     * @brief Draws screen border.
     * @returns None
     */
    void drawBorder();
    
    /**
     * @brief Draws map preview for selection screen.
     * @param map Map information to preview.
     * @returns None
     */
    void drawMapPreview(const MapInfo& map);

    bool useAdvancedGraphics;  ///< Whether to use advanced Unicode graphics

public:
    /**
     * @brief Constructs a ConsoleRenderer object.
     * @returns None
     */
    ConsoleRenderer();
    
    /**
     * @brief Checks if terminal size meets requirements.
     * @return true if terminal size is valid, false otherwise.
     */
    bool checkTerminalSize();
    
    /**
     * @brief Draws an error message about terminal size.
     * @param message Error message to display.
     * @returns None
     */
    void drawErrorMessage(const std::string& message);
    
    /**
     * @brief Clears the terminal screen.
     * @returns None
     */
    void clearScreen();
    
    /**
     * @brief Sets cursor position in terminal.
     * @param x X-coordinate (column).
     * @param y Y-coordinate (row).
     * @returns None
     */
    void setCursorPosition(int x, int y);
    
    /**
     * @brief Renders the game world to console.
     * @param world GameWorld object to render.
     * @return true if rendering successful, false otherwise.
     */
    bool render(const GameWorld& world);
    
    /**
     * @brief Draws symbol legend explaining game symbols.
     * @returns None
     */
    void drawSymbolLegend();
    
    /**
     * @brief Draws main menu screen.
     * @return true if rendering successful, false otherwise.
     */
    bool drawMenu();
    
    /**
     * @brief Draws pause screen.
     * @return true if rendering successful, false otherwise.
     */
    bool drawPauseScreen();
    
    /**
     * @brief Draws game over screen.
     * @param score Final player score.
     * @return true if rendering successful, false otherwise.
     */
    bool drawGameOver(int score);
    
    /**
     * @brief Draws settings screen.
     * @return true if rendering successful, false otherwise.
     */
    bool drawSettings();
    
    /**
     * @brief Draws map selection screen.
     * @param currentMap Currently selected map.
     * @param currentIndex Current map index.
     * @param totalMaps Total number of maps.
     * @return true if rendering successful, false otherwise.
     */
    bool drawMapSelection(const MapInfo& currentMap, int currentIndex, int totalMaps);
    
    /**
     * @brief Draws level completion screen.
     * @param score Current player score.
     * @param level Completed level number.
     * @param lives Remaining player lives.
     * @return true if rendering successful, false otherwise.
     */
    bool drawLevelComplete(int score, int level, int lives);

    /**
     * @brief Enables or disables advanced graphics.
     * @param enabled Whether advanced graphics should be enabled.
     * @returns None
     */
    void setAdvancedGraphics(bool enabled);
    
    /**
     * @brief Checks if advanced graphics are enabled.
     * @return true if advanced graphics enabled, false otherwise.
     */
    bool getAdvancedGraphics() const;

    /**
     * @brief Checks if terminal size is valid.
     * @return true if terminal size meets requirements, false otherwise.
     */
    bool isTerminalSizeValid() const;

    /**
     * @brief Calculates horizontal offset to center content based on terminal width.
     * @param contentWidth Width of content to center.
     * @return Horizontal offset in characters.
     */
    int calculateHorizontalOffset(int contentWidth) const;
};

#endif // CONSOLERENDERER_H
