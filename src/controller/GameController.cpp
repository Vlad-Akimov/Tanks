#ifndef GAMECONTROLLER_CPP
#define GAMECONTROLLER_CPP

#include "../model/GameWorld.cpp"
#include "../view/ConsoleRenderer.cpp"

#include <map>
#include <vector>
#include <iostream>
#include <conio.h>
#include <ctime>

enum class Command { 
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, 
    FIRE, PAUSE, MENU, CONFIRM, BACK, EXIT 
};

class InputHandler {
    private:
        std::map<int, Command> keyBindings;

    public:
        InputHandler() {
            // Стандартные привязки клавиш
            keyBindings['w'] = Command::MOVE_UP;
            keyBindings['W'] = Command::MOVE_UP;
            keyBindings['s'] = Command::MOVE_DOWN;
            keyBindings['S'] = Command::MOVE_DOWN;
            keyBindings['a'] = Command::MOVE_LEFT;
            keyBindings['A'] = Command::MOVE_LEFT;
            keyBindings['d'] = Command::MOVE_RIGHT;
            keyBindings['D'] = Command::MOVE_RIGHT;
            keyBindings[' '] = Command::FIRE;
            keyBindings['f'] = Command::FIRE;
            keyBindings['F'] = Command::FIRE;
            keyBindings['p'] = Command::PAUSE;
            keyBindings['P'] = Command::PAUSE;
            keyBindings['m'] = Command::MENU;
            keyBindings['M'] = Command::MENU;
            keyBindings['\r'] = Command::CONFIRM; // Enter
            keyBindings[27] = Command::BACK; // Escape
            keyBindings['q'] = Command::EXIT;
            keyBindings['Q'] = Command::EXIT;
        }
        
        std::vector<Command> handleEvents() {
            std::vector<Command> commands;
            
            if (_kbhit()) {
                int key = _getch();
                
                // Обработка специальных клавиш
                if (key == 0 || key == 224) {
                    // Специальные клавиши (стрелки и т.д.)
                    key = _getch();
                    switch (key) {
                        case 72: // Стрелка вверх
                            commands.push_back(Command::MOVE_UP);
                            break;
                        case 80: // Стрелка вниз
                            commands.push_back(Command::MOVE_DOWN);
                            break;
                        case 75: // Стрелка влево
                            commands.push_back(Command::MOVE_LEFT);
                            break;
                        case 77: // Стрелка вправо
                            commands.push_back(Command::MOVE_RIGHT);
                            break;
                    }
                } else {
                    // Обычные клавиши
                    auto it = keyBindings.find(key);
                    if (it != keyBindings.end()) {
                        commands.push_back(it->second);
                    }
                }
            }
            
            return commands;
        }
        
        void remapKey(int keyCode, Command command) {
            keyBindings[keyCode] = command;
        }
        
        static int getKeyCode(char c) {
            return static_cast<int>(c);
        }
};

class GameController {
    private:
        GameWorld model;
        ConsoleRenderer view;
        InputHandler inputHandler;
        bool running;

        void handleInput() {
            auto commands = inputHandler.handleEvents();
            
            for (Command cmd : commands) {
                processCommand(cmd);
                
                // Если команда требует немедленного обновления (например, движение или выстрел)
                if (cmd == Command::MOVE_UP || cmd == Command::MOVE_DOWN || 
                    cmd == Command::MOVE_LEFT || cmd == Command::MOVE_RIGHT ||
                    cmd == Command::FIRE) {
                    
                    // Завершаем ход после действия игрока
                    processGameLogic();
                }
            }
        }

        void processGameLogic() {
            if (model.getState() == GameState::PLAYING) {
                // Обновляем игровой мир (движение врагов, проверка столкновений и т.д.)
                model.update();
                
                // Проверяем условия завершения игры
                if (model.getState() == GameState::GAME_OVER) {
                    view.drawGameOver(model.getPlayer()->getScore());
                }
            }
        }

        void processCommand(Command cmd) {
            switch (cmd) {
                case Command::MOVE_UP:
                    if (model.getState() == GameState::PLAYING) {
                        model.playerMove(Direction::UP);
                    }
                    break;
                    
                case Command::MOVE_DOWN:
                    if (model.getState() == GameState::PLAYING) {
                        model.playerMove(Direction::DOWN);
                    }
                    break;
                    
                case Command::MOVE_LEFT:
                    if (model.getState() == GameState::PLAYING) {
                        model.playerMove(Direction::LEFT);
                    }
                    break;
                    
                case Command::MOVE_RIGHT:
                    if (model.getState() == GameState::PLAYING) {
                        model.playerMove(Direction::RIGHT);
                    }
                    break;
                    
                case Command::FIRE:
                    if (model.getState() == GameState::PLAYING) {
                        model.playerFire();
                    }
                    break;
                    
                case Command::PAUSE:
                    pauseGame();
                    break;
                    
                case Command::MENU:
                    if (model.getState() == GameState::PLAYING) {
                        model.setState(GameState::PAUSED);
                    }
                    showMenu();
                    break;
                    
                case Command::CONFIRM:
                    if (model.getState() == GameState::MENU) {
                        // Начать игру
                        model.loadLevel(1);
                    } else if (model.getState() == GameState::PAUSED) {
                        // Продолжить игру
                        model.setState(GameState::PLAYING);
                    }
                    break;
                    
                case Command::BACK:
                    if (model.getState() == GameState::SETTINGS) {
                        model.setState(GameState::MENU);
                    } else if (model.getState() == GameState::PAUSED) {
                        model.setState(GameState::PLAYING);
                    }
                    break;
                    
                case Command::EXIT:
                    running = false;
                    break;
            }
        }

        void showMenu() {
            model.setState(GameState::MENU);
            bool inMenu = true;
            
            while (inMenu && running) {
                view.clearScreen();
                view.drawMenu();
                
                auto commands = inputHandler.handleEvents();
                for (Command cmd : commands) {
                    switch (cmd) {
                        case Command::CONFIRM:
                            // Начать игру
                            model.loadLevel(1);
                            inMenu = false;
                            break;
                            
                        case Command::MENU:
                            showSettings();
                            break;
                            
                        case Command::EXIT:
                            running = false;
                            inMenu = false;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
        }

        void showSettings() {
            model.setState(GameState::SETTINGS);
            bool inSettings = true;
            
            while (inSettings && running) {
                view.clearScreen();
                view.drawSettings();
                
                auto commands = inputHandler.handleEvents();
                for (Command cmd : commands) {
                    switch (cmd) {
                        case Command::BACK:
                            inSettings = false;
                            break;
                            
                        case Command::EXIT:
                            running = false;
                            inSettings = false;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
            
            model.setState(GameState::MENU);
        }

    public:
        GameController(int width, int height) 
            : model(width, height), view(width, height), running(true) {
            // Инициализация случайного генератора
            srand(static_cast<unsigned int>(time(nullptr)));
        }
        
        void runGame() {
            // Показываем главное меню при запуске
            showMenu();
            
            // Основной игровой цикл
            while (running) {
                // Очистка экрана
                view.clearScreen();
                
                // Отрисовка текущего состояния
                switch (model.getState()) {
                    case GameState::PLAYING:
                        view.render(model);
                        std::cout << "Введите команду (WASD - движение, SPACE/F - выстрел, P - пауза, Q - выход): ";
                        break;
                        
                    case GameState::PAUSED:
                        view.drawPauseScreen();
                        break;
                        
                    case GameState::GAME_OVER:
                        view.drawGameOver(model.getPlayer()->getScore());
                        std::cout << "Нажмите M для возврата в меню или Q для выхода: ";
                        break;
                        
                    default:
                        break;
                }
                
                // Обработка ввода
                handleInput();
                
                // Небольшая задержка для уменьшения нагрузки на CPU
                #ifdef _WIN32
                    Sleep(50);
                #else
                    usleep(50000);
                #endif
            }
            
            // Завершение игры
            view.clearScreen();
            std::cout << "Игра завершена. Спасибо за игру!" << std::endl;
        }
        
        void pauseGame() {
            if (model.getState() == GameState::PLAYING) {
                model.setState(GameState::PAUSED);
                view.drawPauseScreen();
                
                // Ожидание снятия с паузы
                bool paused = true;
                while (paused && running) {
                    auto commands = inputHandler.handleEvents();
                    for (Command cmd : commands) {
                        if (cmd == Command::PAUSE || cmd == Command::CONFIRM) {
                            model.setState(GameState::PLAYING);
                            paused = false;
                        } else if (cmd == Command::EXIT) {
                            running = false;
                            paused = false;
                        }
                    }
                    
                    #ifdef _WIN32
                        Sleep(50);
                    #else
                        usleep(50000);
                    #endif
                }
            }
        }
        
        void saveSettings() {
            // Сохранение настроек управления и т.д.
            // В реальной реализации здесь можно сохранять в файл
        }
};

#endif // GAMECONTROLLER_CPP