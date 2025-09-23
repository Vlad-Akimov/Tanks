#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <map>

enum class Command {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    FIRE,
    PAUSE,
    MENU,
    EXIT,
    NONE
};

class InputHandler {
private:
    std::map<char, Command> keyBindings;

public:
    InputHandler();
    
    // Обработка ввода
    Command getInput();
    void remapKey(char key, Command command);
    void loadKeyBindings();
    void saveKeyBindings();
};

#endif // INPUTHANDLER_H