// src/utils/PlatformUtils.h
#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <string>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
#endif

class PlatformUtils {
public:
    static void clearScreen();
    static void setCursorPosition(int x, int y);
    static bool kbhit();
    static std::string readUTF8Char();
    static void sleep(int milliseconds);
    
    // Цветовая поддержка для консоли
    enum class Color {
        DEFAULT = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE = 7
    };
    
    static void setColor(Color color);
    static void resetColor();
    
private:
    
#ifdef _WIN32
    static HANDLE hConsole;
#else
    static struct termios oldt;
#endif
};

#endif // PLATFORMUTILS_H
