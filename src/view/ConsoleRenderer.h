#ifndef CONSOLERENDERER_H
#define CONSOLERENDERER_H

#include <vector>
#include <string>

class ConsoleRenderer {
public:
    static void drawChar(int x, int y, char ch, int color = 7);
    static void drawString(int x, int y, const std::string& text);
    static void drawRect(int x, int y, int width, int height, char borderChar);
    static void clear();
    static void setCursorPosition(int x, int y);
};

#endif