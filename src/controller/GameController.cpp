#ifndef GAMECONTROLLER_CPP
#define GAMECONTROLLER_CPP

#include "../model/GameWorld.cpp"
#include "../view/ConsoleRenderer.cpp"

#include <map>
#include <vector>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <ctime>

enum class Command { 
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, 
    FIRE, PAUSE, MENU, CONFIRM, BACK, EXIT, NONE
};

class InputHandler {
private:
    std::map<int, Command> keyBindings;

    // Кроссплатформенная функция получения символа
    int getch() {
#ifdef _WIN32
        return _getch();
#else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
#endif
    }

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
        keyBindings['\n'] = Command::CONFIRM; // Enter
        keyBindings[27] = Command::BACK; // Escape
        keyBindings['q'] = Command::EXIT;
        keyBindings['Q'] = Command::EXIT;
    }
    
    // НОВЫЙ МЕТОД: ожидание одной команды от пользователя
    Command waitForCommand() {
        int key = getch();
        
#ifdef _WIN32
        // Обработка специальных клавиш для Windows
        if (key == 0 || key == 224) {
            key = getch();
            switch (key) {
                case 72: // Стрелка вверх
                    return Command::MOVE_UP;
                case 80: // Стрелка вниз
                    return Command::MOVE_DOWN;
                case 75: // Стрелка влево
                    return Command::MOVE_LEFT;
                case 77: // Стрелка вправо
                    return Command::MOVE_RIGHT;
                default:
                    return Command::NONE;
            }
        } else {
#else
        // Обработка специальных клавиш для Linux/macOS
        if (key == 27) { // Escape sequence
            if (getch() == 91) { // [
                int key3 = getch();
                switch (key3) {
                    case 65: // Стрелка вверх
                        return Command::MOVE_UP;
                    case 66: // Стрелка вниз
                        return Command::MOVE_DOWN;
                    case 68: // Стрелка влево
                        return Command::MOVE_LEFT;
                    case 67: // Стрелка вправо
                        return Command::MOVE_RIGHT;
                    default:
                        return Command::NONE;
                }
            }
            return Command::BACK;
        } else {
#endif
            // Обычные клавиши
            auto it = keyBindings.find(key);
            if (it != keyBindings.end()) {
                return it->second;
            }
        }
        
        return Command::NONE;
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
        bool waitingForInput;

        // НОВЫЙ МЕТОД: Основной цикл обработки одного хода
        void processGameTurn() {
            // Фаза ВЫВОДА: отрисовка текущего состояния
            view.clearScreen();
            
            switch (model.getState()) {
                case GameState::PLAYING:
                    view.render(model);
                    std::cout << "Введите команду (WASD - движение, SPACE/F - выстрел, P - пауза, M - меню, Q - выход): ";
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
            
            std::cout.flush();
            
            // Фаза ВВОДА: ожидание команды от пользователя
            Command cmd = inputHandler.waitForCommand();
            
            // Фаза ОБРАБОТКИ: выполнение команды и обновление игрового мира
            processCommand(cmd);
            
            // Если мы в игровом режиме и команда была игровой - обновляем мир
            if (model.getState() == GameState::PLAYING && 
                (cmd == Command::MOVE_UP || cmd == Command::MOVE_DOWN || 
                 cmd == Command::MOVE_LEFT || cmd == Command::MOVE_RIGHT ||
                 cmd == Command::FIRE)) {
                
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
                    
                case Command::NONE:
                    // Ничего не делаем для нераспознанных команд
                    break;
            }
        }

        void showMenu() {
            model.setState(GameState::MENU);
            bool inMenu = true;
            
            while (inMenu && running) {
                view.clearScreen();
                view.drawMenu();
                std::cout << "Введите команду: ";
                std::cout.flush();
                
                Command cmd = inputHandler.waitForCommand();
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

        void showSettings() {
            model.setState(GameState::SETTINGS);
            bool inSettings = true;
            
            while (inSettings && running) {
                view.clearScreen();
                view.drawSettings();
                std::cout << "Введите команду: ";
                std::cout.flush();
                
                Command cmd = inputHandler.waitForCommand();
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
            
            model.setState(GameState::MENU);
        }

    public:
        GameController(int width, int height) 
            : model(width, height), view(width, height), running(true), waitingForInput(false) {
            // Инициализация случайного генератора
            srand(static_cast<unsigned int>(time(nullptr)));
        }
        
        void runGame() {
            // Показываем главное меню при запуске
            showMenu();
            
            // Основной игровой цикл - теперь строго пошаговый
            while (running) {
                processGameTurn();
            }
            
            // Завершение игры
            view.clearScreen();
            std::cout << "Игра завершена. Спасибо за игру!" << std::endl;
        }
        
        void pauseGame() {
            if (model.getState() == GameState::PLAYING) {
                model.setState(GameState::PAUSED);
                
                bool paused = true;
                while (paused && running) {
                    view.clearScreen();
                    view.drawPauseScreen();
                    std::cout << "Введите команду: ";
                    std::cout.flush();
                    
                    Command cmd = inputHandler.waitForCommand();
                    if (cmd == Command::PAUSE || cmd == Command::CONFIRM) {
                        model.setState(GameState::PLAYING);
                        paused = false;
                    } else if (cmd == Command::EXIT) {
                        running = false;
                        paused = false;
                    } else if (cmd == Command::MENU) {
                        showMenu();
                        paused = false;
                    }
                }
            }
        }
        
        void saveSettings() {
            // Сохранение настроек управления и т.д.
        }
};

#endif // GAMECONTROLLER_CPP