#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "../utils/PlatformUtils.h"

#include <map>
#include <string>

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

#endif // INPUTHANDLER_H
