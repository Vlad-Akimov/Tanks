#include <iostream>
#include "controller/GameController.h"

int main() {
    try {
        GameController game(40, 20);
        game.runGame();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}


// проверка совместимости