#include <iostream>
#include "controller/GameController.h"
#include <algorithm>

int main() {
    try {
        const int MIN_WIDTH = 40;
        const int MIN_HEIGHT = 40;
        const int WIDTH = 40;
        const int HEIGHT = 20;

        if (!PlatformUtils::isTerminalSizeValid(MIN_WIDTH, MIN_HEIGHT)) {
            PlatformUtils::showResizeMessage(MIN_WIDTH, MIN_HEIGHT);
            return -1;
        }

        auto [termWidth, termHeight] = PlatformUtils::getTerminalSize();

        int gameWidth = std::min(WIDTH, termWidth);
        int gameHeight = std::min(HEIGHT, termHeight);
        
        GameController game(gameWidth, gameHeight);
        
        game.runGame();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
