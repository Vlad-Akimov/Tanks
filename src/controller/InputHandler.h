// src/controller/InputHandler.h
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <map>
#include <vector>
#include <functional>
#include "Command.h"
#include "../utils/Constants.h"

class InputHandler {
private:
    std::map<int, Command> keyBindings; // key code to command mapping
    std::map<Constants::GameState, std::function<void(Command)>> commandHandlers;
    bool exitRequested;

public:
    InputHandler();
    ~InputHandler() = default;
    
    void initializeDefaultBindings();
    std::vector<Command> handleInput();
    void remapKey(int keyCode, Command command);
    void setCommandHandler(Constants::GameState state, std::function<void(Command)> handler);
    
    void setExitRequested(bool exit) { exitRequested = exit; }
    bool getExitRequested() const { return exitRequested; }
    
    // Key detection methods (platform-specific implementations would be in .cpp)
    bool isKeyPressed(int keyCode) const;
    int getPressedKey() const;
};

#endif