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
#include <windows.h>
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
        std::map<std::string, Command> utf8KeyBindings;
    
    #ifdef _WIN32
        // Для Windows: функция чтения UTF-8 символов
        std::string readUTF8Char() {
            std::string result;
            int ch = _getch();
            
            // Если это начало UTF-8 последовательности
            if ((ch & 0xE0) == 0xC0) { // 2-byte sequence
                result += static_cast<char>(ch);
                result += static_cast<char>(_getch());
            }
            else if ((ch & 0xF0) == 0xE0) { // 3-byte sequence
                result += static_cast<char>(ch);
                result += static_cast<char>(_getch());
                result += static_cast<char>(_getch());
            }
            else if ((ch & 0xF8) == 0xF0) { // 4-byte sequence
                result += static_cast<char>(ch);
                result += static_cast<char>(_getch());
                result += static_cast<char>(_getch());
                result += static_cast<char>(_getch());
            }
            else {
                // Одиночный ASCII символ
                result += static_cast<char>(ch);
            }
            
            return result;
        }
    #else
        // Для Linux/macOS: проверка, доступен ли символ для чтения
        bool kbhit() {
            struct termios oldt, newt;
            int ch;
            int oldf;
    
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
            ch = getchar();
    
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            fcntl(STDIN_FILENO, F_SETFL, oldf);
    
            if(ch != EOF) {
                ungetc(ch, stdin);
                return true;
            }
    
            return false;
        }
    
        // Для Linux/macOS: функция чтения UTF-8 символов
        std::string readUTF8Char() {
            std::string result;
            struct termios oldt, newt;
            
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            
            // Читаем первый байт
            int firstByte = getchar();
            if (firstByte == EOF) {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                return result;
            }
            
            result += static_cast<char>(firstByte);
            
            // Определяем длину UTF-8 последовательности по первому байту
            int numBytes = 1;
            if ((firstByte & 0xE0) == 0xC0) numBytes = 2;
            else if ((firstByte & 0xF0) == 0xE0) numBytes = 3;
            else if ((firstByte & 0xF8) == 0xF0) numBytes = 4;
            
            // Читаем оставшиеся байты
            for (int i = 1; i < numBytes; ++i) {
                if (!kbhit()) {
                    // Если данных нет, ждем небольшое время
                    usleep(1000);
                }
                int nextByte = getchar();
                if (nextByte != EOF) {
                    result += static_cast<char>(nextByte);
                }
            }
            
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return result;
        }
    #endif
    
    public:
        InputHandler() {
            // Стандартные привязки клавиш для латиницы
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
    
            // Привязки для кириллицы (UTF-8)
            utf8KeyBindings["ц"] = Command::MOVE_UP;      // w -> ц
            utf8KeyBindings["Ц"] = Command::MOVE_UP;      // W -> Ц
            utf8KeyBindings["ы"] = Command::MOVE_DOWN;    // s -> ы
            utf8KeyBindings["Ы"] = Command::MOVE_DOWN;    // S -> Ы
            utf8KeyBindings["ф"] = Command::MOVE_LEFT;    // a -> ф
            utf8KeyBindings["Ф"] = Command::MOVE_LEFT;    // A -> Ф
            utf8KeyBindings["в"] = Command::MOVE_RIGHT;   // d -> в
            utf8KeyBindings["В"] = Command::MOVE_RIGHT;   // D -> В
            utf8KeyBindings["а"] = Command::FIRE;         // f -> а
            utf8KeyBindings["А"] = Command::FIRE;         // F -> А
            utf8KeyBindings["з"] = Command::PAUSE;        // p -> з
            utf8KeyBindings["З"] = Command::PAUSE;        // P -> З
            utf8KeyBindings["ь"] = Command::MENU;         // m -> ь
            utf8KeyBindings["Ь"] = Command::MENU;         // M -> Ь
            utf8KeyBindings["й"] = Command::EXIT;         // q -> й
            utf8KeyBindings["Й"] = Command::EXIT;         // Q -> Й
        }
        
        // НОВЫЙ МЕТОД: ожидание одной команды от пользователя
        Command waitForCommand() {
    #ifdef _WIN32
            // Для Windows используем нашу функцию чтения UTF-8
            std::string input = readUTF8Char();
            if (input.length() == 1) {
                int key = static_cast<unsigned char>(input[0]);
    #else
            // Для Linux/macOS сначала проверяем специальные клавиши
            struct termios oldt, newt;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            
            int firstByte = getchar();
            
            // Обработка специальных клавиш для Linux/macOS
            if (firstByte == 27) { // Escape sequence
                if (kbhit()) {
                    int secondByte = getchar();
                    if (secondByte == 91) { // [
                        if (kbhit()) {
                            int thirdByte = getchar();
                            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                            switch (thirdByte) {
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
                    }
                }
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                return Command::BACK;
            }
            
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            
            // Если это не escape sequence, обрабатываем как UTF-8
            std::string input;
            input += static_cast<char>(firstByte);
            
            // Определяем длину UTF-8 последовательности
            int numBytes = 1;
            if ((firstByte & 0xE0) == 0xC0) numBytes = 2;
            else if ((firstByte & 0xF0) == 0xE0) numBytes = 3;
            else if ((firstByte & 0xF8) == 0xF0) numBytes = 4;
            
            // Читаем оставшиеся байты если нужно
            for (int i = 1; i < numBytes && kbhit(); ++i) {
                int nextByte = getchar();
                if (nextByte != EOF) {
                    input += static_cast<char>(nextByte);
                }
            }
            
            if (input.length() == 1) {
                int key = static_cast<unsigned char>(input[0]);
    #endif
                // Обработка специальных клавиш для Windows
                if (key == 0 || key == 224) {
                    int specialKey = getchar();
                    switch (specialKey) {
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
                }
                
                // Обычные клавиши латиницы
                auto it = keyBindings.find(key);
                if (it != keyBindings.end()) {
                    return it->second;
                }
            }
            
            // Проверяем UTF-8 последовательности (кириллица)
            auto utf8It = utf8KeyBindings.find(input);
            if (utf8It != utf8KeyBindings.end()) {
                return utf8It->second;
            }
            
            return Command::NONE;
        }
        
        void remapKey(int keyCode, Command command) {
            keyBindings[keyCode] = command;
        }
        
        void remapUTF8Key(const std::string& utf8Char, Command command) {
            utf8KeyBindings[utf8Char] = command;
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
                    std::cout << "Введите команду (ENTER - новая игра, M - меню, Q - выход): ";
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
                    } else if (model.getState() == GameState::GAME_OVER) {
                        // Возврат в меню после Game Over
                        showMenu();
                    }
                    break;
                    
                case Command::CONFIRM:
                    if (model.getState() == GameState::MENU) {
                        // Начать новую игру - сбросить всё состояние
                        model.loadLevel(1);
                    } else if (model.getState() == GameState::PAUSED) {
                        // Продолжить игру
                        model.setState(GameState::PLAYING);
                    } else if (model.getState() == GameState::GAME_OVER) {
                        // Начать новую игру после Game Over
                        model.loadLevel(1);
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
            : model(width, height), view(width, height), running(true) {
            // Инициализация случайного генератора
            srand(static_cast<unsigned int>(time(nullptr)));
        }
        
        void runGame() {
            showMenu();
            
            while (running) { processGameTurn(); }
            
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