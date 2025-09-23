#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <map>
#include <vector>

enum class Command { 
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, 
    FIRE, PAUSE, MENU, CONFIRM, BACK, EXIT 
};

class InputHandler {
private:
    std::map<int, Command> keyBindings; // int для кодов клавиш

public:
    InputHandler();
    
    // Обработка ввода
    std::vector<Command> handleEvents();
    void remapKey(int keyCode, Command command);
    
    // Статические методы для кодов клавиш
    static int getKeyCode(char c);
};

#endif // INPUTHANDLER_H