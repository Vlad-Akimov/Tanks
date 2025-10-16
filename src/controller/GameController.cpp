#ifndef GAMECONTROLLER_CPP
#define GAMECONTROLLER_CPP

#include "GameController.h"
#include <ctime>

GameController::GameController(int width, int height) 
    : model(width, height), view(width, height), running(true), 
      mapManager("resources/maps"), currentMapIndex(0) {
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
                model.setState(GameState::GAME_OVER);
                showMenu();
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
        
        // Создаем игровой мир с размерами выбранной карты
        model = GameWorld(selectedMap.width, selectedMap.height);
        
        // Загружаем объекты из карты
        mapManager.createWorldFromMap(model, selectedMap);
        
        // Загружаем уровень (это добавит врагов и настроит игрока)
        model.loadLevel(1);
        
        std::cout << "Загружена карта: " << selectedMap.displayName << std::endl;
    } else {
        // Если что-то пошло не так, используем стандартную карту
        model.loadLevel(1);
    }
}

#endif // GAMECONTROLLER_CPP
