#include <iostream>
#include "controller/GameController.h"

int main() {
    try {
        GameController game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}