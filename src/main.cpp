#include <iostream>
#include "controller/GameController.h"

int main() {
    try {
        const int WIDTH = 40;
        const int HEIGHT = 20;

        GameController game(WIDTH, HEIGHT);
        game.runGame();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
