#include "GameController.h"
#include <ctime>

GameController::GameController(int width, int height) 
    : model(width, height), view(width, height), running(true), 
      mapManager("../resources/maps"), currentMapIndex(0), 
      useCustomMap(false) {
    srand(static_cast<unsigned int>(time(nullptr)));
    mapManager.loadMaps();
}

void GameController::showMapSelection() {
    bool inMapSelection = true;
    
    if (mapManager.getMapCount() == 0) {
        mapManager.loadMaps();
    }
    
    if (mapManager.getMapCount() == 0) {
        std::cout << "Карты не найдены! Используется случайная генерация." << std::endl;
        useCustomMap = false;
        model.loadLevel(1);
        return;
    }
    
    while (inMapSelection && running) {
        // Получаем текущую карту
        const MapInfo& currentMap = mapManager.getMap(currentMapIndex);
        
        // Отрисовываем экран выбора карты
        view.clearScreen();
        view.drawMapSelection(currentMap, currentMapIndex, mapManager.getMapCount());
        std::cout.flush();
        
        Command cmd = inputHandler.waitForCommand();
        switch (cmd) {
            case Command::MOVE_LEFT: // A
                currentMapIndex--;
                if (currentMapIndex < 0) {
                    currentMapIndex = mapManager.getMapCount() - 1;
                }
                break;
                
            case Command::MOVE_RIGHT: // D
                currentMapIndex++;
                if (currentMapIndex >= mapManager.getMapCount()) {
                    currentMapIndex = 0;
                }
                break;
                
            case Command::FIRE: // R - случайная генерация
                useCustomMap = false;
                model.loadLevel(1);
                inMapSelection = false;
                break;
                
            case Command::CONFIRM: // ENTER - начать игру с выбранной картой
                useCustomMap = true;
                loadSelectedMap();
                inMapSelection = false;
                break;
                
            case Command::MENU: // M - вернуться в главное меню
                inMapSelection = false;
                showMenu();
                return;
                
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
        const MapInfo& map = mapManager.getMap(currentMapIndex);
        
        std::cout << "Загрузка карты: " << map.displayName << std::endl;
        std::cout << "Размер: " << map.width << "x" << map.height << std::endl;
        
        // Сохраняем выбранную карту
        selectedMap = map;
        useCustomMap = true;
        
        // СОХРАНЯЕМ состояние игрока перед созданием нового мира
        int currentLevel = model.getCurrentLevel();
        PlayerTank* oldPlayer = model.getPlayer();
        int savedScore = oldPlayer ? oldPlayer->getScore() : 0;
        int savedLives = oldPlayer ? oldPlayer->getLives() : 3;
        
        // Создаем новый игровой мир с размерами выбранной карты
        model = GameWorld(selectedMap.width, selectedMap.height);
        
        // ВОССТАНАВЛИВАЕМ состояние игрока
        model.setCurrentLevel(currentLevel);
        PlayerTank* newPlayer = model.getPlayer();
        if (newPlayer && oldPlayer) {
            newPlayer->setScore(savedScore);
            newPlayer->setLives(savedLives);
            newPlayer->setHealth(3);
        }
        
        // Загружаем объекты из карты
        mapManager.createWorldFromMap(model, selectedMap);
        
        // Устанавливаем состояние игры
        model.setState(GameState::PLAYING);
        
        std::cout << "Карта успешно загружена: " << selectedMap.displayName 
                  << " Уровень: " << currentLevel << " Счет: " << savedScore << std::endl;
    } else {
        // Если что-то пошло не так, используем стандартную карту
        std::cout << "Ошибка загрузки карты, используется случайная генерация" << std::endl;
        useCustomMap = false;
        model.loadLevel(model.getCurrentLevel()); // Загружаем текущий уровень, а не сбрасываем
    }
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
            std::cout.flush();
            
            Command cmd = inputHandler.waitForCommand();
            if (cmd == Command::PAUSE || cmd == Command::CONFIRM) {
                model.setState(GameState::PLAYING);
                paused = false;
            } else if (cmd == Command::EXIT) {
                running = false;
                paused = false;
            } else if (cmd == Command::MENU) {
                model.getPlayer()->setScore(0);
                model.getPlayer()->setLives(3);
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
    int score = 0;
    
    switch (model.getState()) {
        case GameState::PLAYING:
            view.render(model);
            
            if (model.getEnemyCount() == 0) {
                loadNextLevel();
            }
            break;
            
        case GameState::PAUSED:
            view.drawPauseScreen();
            break;
            
        case GameState::GAME_OVER:
            if (model.getPlayer()) {
                score = model.getPlayer()->getScore();
                if (score > 0) {
                    scoreManager.addScore(score);
                }
            }
            view.drawGameOver(score);
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
                if (model.getPlayer() && model.getPlayer()->getScore() > 0) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                }
                model.setState(GameState::GAME_OVER);
                model.setCurrentLevel(1);
                showMenu();
            } else if (model.getState() == GameState::GAME_OVER) {
                model.setCurrentLevel(1);
                showMenu();
            }
            if (model.getPlayer()) {
                model.getPlayer()->setLives(3);
            }
            break;
            
        case Command::CONFIRM:
            if (model.getState() == GameState::MENU) {
                useCustomMap = false;
                model.setCurrentLevel(1);
                if (model.getPlayer()) {
                    model.getPlayer()->setScore(0);
                    model.getPlayer()->setLives(3);
                }
                showMapSelection();
            } else if (model.getState() == GameState::PAUSED) {
                model.setState(GameState::PLAYING);
            } else if (model.getState() == GameState::GAME_OVER) {
                if (model.getPlayer() && model.getPlayer()->getScore() > 0) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                }
                useCustomMap = false;
                model.setCurrentLevel(1);
                if (model.getPlayer()) {
                    model.getPlayer()->setScore(0);
                    model.getPlayer()->setLives(3);
                }
                showMapSelection();
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
            break;
    }
}

void GameController::showMenu() {
    model.setState(GameState::MENU);
    bool inMenu = true;
    
    while (inMenu && running) {
        view.clearScreen();
        view.drawMenu();
        
        std::cout << "\n=== ТАБЛИЦА РЕКОРДОВ ===\n";
        std::vector<int> highScores = scoreManager.getHighScores();
        if (highScores.empty()) {
            std::cout << "   Пока нет рекордов!\n";
        } else {
            for (size_t i = 0; i < highScores.size(); i++) {
                std::cout << "   " << (i + 1) << ". " << highScores[i] << " очков\n";
            }
        }
        std::cout << "=======================\n\n";
        
        std::cout.flush();
        
        Command cmd = inputHandler.waitForCommand();
        switch (cmd) {
            case Command::CONFIRM:
                model.setCurrentLevel(1);
                model.getPlayer()->setScore(0);
                model.getPlayer()->setLives(3);
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

void GameController::loadNextLevel() {
    int nextLevel = model.getCurrentLevel();
    
    if (useCustomMap) {
        std::cout << "Загрузка уровня " << nextLevel << " на выбранной карте" << std::endl;
        loadSelectedMap();
    } else {
        std::cout << "Загрузка случайно сгенерированного уровня " << nextLevel << std::endl;
        model.loadLevel(nextLevel);
    }
}
