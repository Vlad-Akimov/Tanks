#include <iostream>
#include "controller/GameController.cpp"

int main() {
    try {
        // Создаем и запускаем игру с полем 40x20 клеток
        GameController game(40, 20);
        game.runGame();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}