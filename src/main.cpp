// src/main.cpp
#include <iostream>
#include "controller/GameController.cpp"

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