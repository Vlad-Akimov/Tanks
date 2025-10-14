#ifndef GAMECONTROLLER_CPP
#define GAMECONTROLLER_CPP

#include "GameController.h"
#include <ctime>

// Реализация InputHandler

InputHandler::InputHandler() {
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

Command InputHandler::waitForCommand() {
    std::string input = PlatformUtils::readUTF8Char();
    
    // Обработка специальных клавиш для Linux/macOS
#ifdef _WIN32
    // Для Windows специальные клавиши обрабатываются внутри readUTF8Char
    if (input.length() == 1) {
        int key = static_cast<unsigned char>(input[0]);
        
        // Обработка специальных клавиш для Windows
        if (key == 0 || key == 224) {
            int specialKey = PlatformUtils::readUTF8Char()[0];
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
#else
    // Для Linux/macOS обрабатываем escape sequences для стрелок
    if (input.length() == 1 && static_cast<unsigned char>(input[0]) == 27) {
        if (PlatformUtils::kbhit()) {
            std::string secondChar = PlatformUtils::readUTF8Char();
            if (secondChar == "[") {
                if (PlatformUtils::kbhit()) {
                    std::string thirdChar = PlatformUtils::readUTF8Char();
                    if (thirdChar.length() == 1) {
                        switch (static_cast<unsigned char>(thirdChar[0])) {
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
        }
        return Command::BACK;
    }
    
    // Обычные клавиши латиницы для Linux/macOS
    if (input.length() == 1) {
        int key = static_cast<unsigned char>(input[0]);
        auto it = keyBindings.find(key);
        if (it != keyBindings.end()) {
            return it->second;
        }
    }
#endif
    
    // Проверяем UTF-8 последовательности (кириллица)
    auto utf8It = utf8KeyBindings.find(input);
    if (utf8It != utf8KeyBindings.end()) {
        return utf8It->second;
    }
    
    return Command::NONE;
}

void InputHandler::remapKey(int keyCode, Command command) {
    keyBindings[keyCode] = command;
}

void InputHandler::remapUTF8Key(const std::string& utf8Char, Command command) {
    utf8KeyBindings[utf8Char] = command;
}

int InputHandler::getKeyCode(char c) {
    return static_cast<int>(c);
}

// Реализация GameController

GameController::GameController(int width, int height) 
    : model(width, height), view(width, height), running(true), 
      mapManager("../../resources/maps"), currentMapIndex(0) {
    srand(static_cast<unsigned int>(time(nullptr)));
}

void GameController::runGame() {
    showMenu();
    
    while (running) { processGameTurn(); }
    
    // Завершение игры
    view.clearScreen();
    std::cout << "Игра завершена. Спасибо за игру!" << std::endl;
}

void GameController::pauseGame() {
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

void GameController::saveSettings() {
    // Сохранение настроек управления и т.д.
}

void GameController::processGameTurn() {
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

void GameController::processCommand(Command cmd) {
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

void GameController::showMenu() {
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
                showMapSelection();
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

void GameController::showSettings() {
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

void GameController::showMapSelection() {
    model.setState(GameState::MENU);
    bool inMapSelection = true;
    
    // Загружаем карты
    mapManager.loadMaps();
    
    if (mapManager.getMapCount() == 0) {
        std::cout << "Карты не найдены! Используется случайная генерация." << std::endl;
        model.loadLevel(1); // Используем стандартную генерацию
        return;
    }
    
    while (inMapSelection && running) {
        // Получаем текущую карту
        const MapInfo& currentMap = mapManager.getMap(currentMapIndex);
        
        // Отрисовываем экран выбора карты
        view.clearScreen();
        view.drawMapSelection(currentMap, currentMapIndex, mapManager.getMapCount());
        std::cout << "Введите команду: ";
        std::cout.flush();
        
        Command cmd = inputHandler.waitForCommand();
        switch (cmd) {
            case Command::MOVE_LEFT: // A
                // Предыдущая карта
                currentMapIndex--;
                if (currentMapIndex < 0) {
                    currentMapIndex = mapManager.getMapCount() - 1;
                }
                break;
                
            case Command::MOVE_RIGHT: // D
                // Следующая карта
                currentMapIndex++;
                if (currentMapIndex >= mapManager.getMapCount()) {
                    currentMapIndex = 0;
                }
                break;
                
            case Command::FIRE: // R - случайная генерация
                // Используем стандартную генерацию карты
                model.loadLevel(1);
                inMapSelection = false;
                break;
                
            case Command::CONFIRM: // ENTER - начать игру с выбранной картой
                loadSelectedMap();
                inMapSelection = false;
                break;
                
            case Command::MENU: // M - вернуться в главное меню
                showMenu();
                inMapSelection = false;
                break;
                
            case Command::EXIT: // Q - выход
                running = false;
                inMapSelection = false;
                break;
                
            default:
                break;
        }
    }
}

void GameController::loadSelectedMap() {
    if (mapManager.isValidMapIndex(currentMapIndex)) {
        const MapInfo& selectedMap = mapManager.getMap(currentMapIndex);
        
        // Здесь можно добавить загрузку конкретной карты из файла
        // Пока используем стандартную генерацию, но с размерами выбранной карты
        model = GameWorld(selectedMap.width, selectedMap.height);
        model.loadLevel(1);
        
        std::cout << "Загружена карта: " << selectedMap.displayName << std::endl;
    } else {
        // Если что-то пошло не так, используем стандартную карту
        model.loadLevel(1);
    }
}

#endif // GAMECONTROLLER_CPP
