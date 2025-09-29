#ifndef GAMECONTROLLER_CPP
#define GAMECONTROLLER_CPP

#include "../model/GameWorld.cpp"
#include "../view/ConsoleRenderer.cpp"

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

class GameController {
    private:
        GameWorld model;
        ConsoleRenderer view;
        InputHandler inputHandler;
        bool running;

        // Вспомогательные методы
        void handleInput();
        void processGameLogic();
        void processCommand(Command cmd);
        void showMenu();
        void showSettings();

    public:
        GameController(int width, int height);
        
        // Основной цикл игры
        void runGame();
        void pauseGame();
        void saveSettings();
};

#endif // GAMECONTROLLER_CPP