/**
 * @file InputHandler.h
 * @author Vld251
 * @brief Handles user input and maps keystrokes to game commands.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../utils/PlatformUtils.h"

#include <map>
#include <string>

/**
 * @brief Enumeration representing all possible user commands.
 */
enum class Command { 
    MOVE_UP,      ///< Move player tank up
    MOVE_DOWN,    ///< Move player tank down
    MOVE_LEFT,    ///< Move player tank left
    MOVE_RIGHT,   ///< Move player tank right
    FIRE,         ///< Fire projectile
    PAUSE,        ///< Pause/resume game
    MENU,         ///< Open main menu
    CONFIRM,      ///< Confirm selection/action
    BACK,         ///< Go back/exit current screen
    EXIT,         ///< Exit game
    NONE          ///< No command/unknown input
};

/**
 * @brief Class for handling user input and mapping it to game commands.
 * 
 * Supports both ASCII and UTF-8 input, including Cyrillic characters.
 * Handles special keys like arrow keys and provides key remapping functionality.
 */
class InputHandler {
private:
    std::map<int, Command> keyBindings;          ///< ASCII key bindings
    std::map<std::string, Command> utf8KeyBindings;  ///< UTF-8 key bindings

public:
    /**
     * @brief Constructs an InputHandler with default key bindings.
     * @returns None
     */
    InputHandler();
    
    /**
     * @brief Waits for and returns a user command.
     * @return Detected command from user input.
     */
    Command waitForCommand();
    
    /**
     * @brief Remaps an ASCII key to a command.
     * @param keyCode ASCII key code to remap.
     * @param command Command to assign to the key.
     * @returns None
     */
    void remapKey(int keyCode, Command command);
    
    /**
     * @brief Remaps a UTF-8 character to a command.
     * @param utf8Char UTF-8 character string to remap.
     * @param command Command to assign to the character.
     * @returns None
     */
    void remapUTF8Key(const std::string& utf8Char, Command command);
    
    /**
     * @brief Converts a character to its key code.
     * @param c Character to convert.
     * @return ASCII key code of the character.
     */
    static int getKeyCode(char c);
};

#endif // INPUTHANDLER_H
