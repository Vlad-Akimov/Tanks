#include <iostream>
#include <conio.h>
#include <windows.h>
#include "model/GameWorld.h"
#include "view/ConsoleRenderer.h"
#include "controller/InputHandler.h"

class Game {
private:
    GameWorld world;
    ConsoleRenderer renderer;
    InputHandler inputHandler;
    bool running;
    bool paused;
    GameState currentState;

    enum class GameState {
        MAIN_MENU,
        PLAYING,
        PAUSED,
        GAME_OVER,
        SETTINGS
    };

public:
    Game() : running(true), paused(false), currentState(GameState::MAIN_MENU) {
        world.initializeLevel(1);
    }

    void run() {
        while (running) {
            handleInput();
            update();
            render();
            Sleep(100); // Задержка для стабильного FPS
        }
    }

private:
    void handleInput() {
        Command cmd = inputHandler.getInput();
        
        switch (currentState) {
            case GameState::MAIN_MENU:
                handleMenuInput(cmd);
                break;
            case GameState::PLAYING:
                handleGameInput(cmd);
                break;
            case GameState::PAUSED:
                handlePauseInput(cmd);
                break;
            case GameState::GAME_OVER:
                handleGameOverInput(cmd);
                break;
        }
    }

    void handleMenuInput(Command cmd) {
        switch (cmd) {
            case Command::EXIT:
                running = false;
                break;
            // Обработка меню
        }
    }

    void handleGameInput(Command cmd) {
        switch (cmd) {
            case Command::PAUSE:
                currentState = GameState::PAUSED;
                break;
            case Command::EXIT:
                running = false;
                break;
            // Обработка игровых команд
        }
    }

    void handlePauseInput(Command cmd) {
        // Обработка паузы
    }

    void handleGameOverInput(Command cmd) {
        // Обработка конца игры
    }

    void update() {
        if (currentState == GameState::PLAYING && !paused) {
            world.update();
            
            if (world.isGameOver()) {
                currentState = GameState::GAME_OVER;
            }
        }
    }

    void render() {
        system("cls"); // Очистка консоли (Windows)
        
        switch (currentState) {
            case GameState::MAIN_MENU:
                renderer.drawMenu();
                break;
            case GameState::PLAYING:
                renderer.drawGameWorld(world);
                if (paused) {
                    renderer.drawPauseScreen();
                }
                break;
            case GameState::GAME_OVER:
                renderer.drawGameWorld(world);
                renderer.drawGameOverScreen(world.isLevelCompleted());
                break;
        }
        
        renderer.render();
    }
};

int main() {
    // Настройка консоли
    SetConsoleOutputCP(65001); // UTF-8 для Windows
    
    std::cout << "Танчики - Загрузка..." << std::endl;
    
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Спасибо за игру!" << std::endl;
    return 0;
}