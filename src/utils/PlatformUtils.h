/**
 * @file PlatformUtils.h
 * @author Vld251
 * @brief Cross-platform utilities for console I/O, colors, and terminal manipulation.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <string>

// Platform-specific includes
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
#endif

/**
 * @brief Platform-independent utility class for console operations.
 * 
 * Provides cross-platform functionality for terminal manipulation,
 * input handling, color management, and Unicode support detection.
 */
class PlatformUtils {
public:
    /**
     * @brief Clears the terminal screen.
     * @returns None
     */
    static void clearScreen();
    
    /**
     * @brief Sets cursor position in terminal.
     * @param x X-coordinate (column).
     * @param y Y-coordinate (row).
     * @returns None
     */
    static void setCursorPosition(int x, int y);
    
    /**
     * @brief Checks if a key has been pressed.
     * @return true if key pressed, false otherwise.
     */
    static bool kbhit();
    
    /**
     * @brief Reads a single UTF-8 character from input.
     * @return UTF-8 character as string.
     */
    static std::string readUTF8Char();
    
    /**
     * @brief Pauses execution for specified milliseconds.
     * @param milliseconds Time to sleep in milliseconds.
     * @returns None
     */
    static void sleep(int milliseconds);
    
    /**
     * @brief Enumeration representing terminal colors.
     */
    enum Color {
        BLACK = 0,      ///< Black color
        RED = 1,        ///< Red color
        GREEN = 2,      ///< Green color
        YELLOW = 3,     ///< Yellow color
        BLUE = 4,       ///< Blue color
        MAGENTA = 5,    ///< Magenta color
        CYAN = 6,       ///< Cyan color
        WHITE = 7,      ///< White color
        DEFAULT = 9     ///< Default terminal color
    };
    
    /**
     * @brief Sets terminal text color.
     * @param color Color to set.
     * @returns None
     */
    static void setColor(Color color);
    
    /**
     * @brief Resets terminal color to default.
     * @returns None
     */
    static void resetColor();

    /**
     * @brief Checks if terminal supports Unicode.
     * @return true if Unicode supported, false otherwise.
     */
    static bool supportsUnicode();
    
    /**
     * @brief Checks and reports Unicode support status.
     * @return true if Unicode supported, false otherwise.
     */
    static bool checkUnicodeSupport();

    /**
     * @brief Sets terminal background color.
     * @param color Background color to set.
     * @returns None
     */
    static void setBackgroundColor(Color color);
    
    /**
     * @brief Resets terminal background color to default.
     * @returns None
     */
    static void resetBackgroundColor();

    /**
     * @brief Gets current terminal dimensions.
     * @return Pair of integers (width, height).
     */
    static std::pair<int, int> getTerminalSize();
    
    /**
     * @brief Checks if terminal meets minimum size requirements.
     * @param minWidth Minimum required width.
     * @param minHeight Minimum required height.
     * @return true if terminal size is sufficient, false otherwise.
     */
    static bool isTerminalSizeValid(int minWidth, int minHeight);
    
    /**
     * @brief Displays message about required terminal size.
     * @param requiredWidth Required terminal width.
     * @param requiredHeight Required terminal height.
     * @returns None
     */
    static void showResizeMessage(int requiredWidth, int requiredHeight);
    
private:
    // Platform-specific variables
#ifdef _WIN32
    static HANDLE hConsole;  ///< Windows console handle
#else
    static struct termios oldt;  ///< Unix terminal settings backup
#endif
};

#endif // PLATFORMUTILS_H
