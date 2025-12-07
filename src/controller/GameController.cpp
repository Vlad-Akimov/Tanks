/**
 * @file GameController.cpp
 * @author Vld251
 * @brief Implementation of main game controller and game loop logic.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GameController.h"
#include <ctime>
#include <thread>

GameController::GameController(int width, int height) 
    : model(width, height), view(), running(true), 
      mapManager("../resources/maps"), currentMapIndex(0), 
      scoreSaved(false), useCustomMap(false) {
    srand(static_cast<unsigned int>(time(nullptr)));
    mapManager.loadMaps();

    bool advancedGraphics = settingsManager.getBoolSetting("advanced_graphics", true);
    view.setAdvancedGraphics(advancedGraphics);
}

void GameController::toggleAdvancedGraphics() {
    bool currentSetting = settingsManager.getBoolSetting("advanced_graphics", true);
    bool newSetting = !currentSetting;
    
    settingsManager.setBoolSetting("advanced_graphics", newSetting);
    view.setAdvancedGraphics(newSetting);
    
    // Сообщаем пользователю о изменении
    view.clearScreen();
    std::cout << "Graphics mode changed: " 
          << (newSetting ? "ADVANCED" : "SYMBOLIC") << std::endl;
    PlatformUtils::sleep(1000);
}

void GameController::handleTerminalResize() {
    while (!view.checkTerminalSize() && running) {
        view.clearScreen();
        view.drawErrorMessage("Terminal size too small. Increase window size.");
        
        // Ждем некоторое время перед повторной проверкой
        PlatformUtils::sleep(500);
        
        // Также проверяем команды пользователя во время ожидания
        if (PlatformUtils::kbhit()) {
            std::string input = PlatformUtils::readUTF8Char();
            if (input == "q" || input == "Q" || input == "й" || input == "Й") {
                running = false;
                return;
            }
        }
    }
}

void GameController::showMapSelection() {
    bool inMapSelection = true;
    
    if (mapManager.getMapCount() == 0) {
        mapManager.loadMaps();
    }
    
    if (mapManager.getMapCount() == 0) {
        std::cout << "Maps not found! Using random generation." << std::endl;
        useCustomMap = false;
        model.loadLevel(1);
        model.setState(GameState::PLAYING);
        return;
    }
    
    while (inMapSelection && running) {
        handleTerminalResize();
        if (!running) return;

        const MapInfo& currentMap = mapManager.getMap(currentMapIndex);
        
        view.clearScreen();
        if (!view.drawMapSelection(currentMap, currentMapIndex, mapManager.getMapCount())) {
            continue;
        }
        std::cout.flush();
        
        std::string input = PlatformUtils::readUTF8Char();
        
        if (input == "a" || input == "A" || input == "ф" || input == "Ф") {
            // Предыдущая карта
            currentMapIndex--;
            if (currentMapIndex < 0) {
                currentMapIndex = mapManager.getMapCount() - 1;
            }
        }
        else if (input == "d" || input == "D" || input == "в" || input == "В") {
            // Следующая карта
            currentMapIndex++;
            if (currentMapIndex >= mapManager.getMapCount()) {
                currentMapIndex = 0;
            }
        }
        else if (input == "f" || input == "F" || input == "а" || input == "А") {
            // Случайная генерация
            useCustomMap = false;
            model.loadLevel(1);
            model.setState(GameState::PLAYING);
            inMapSelection = false;
        }
        else if (input == "\n" || input == " ") {
            // Подтверждение выбора карты
            useCustomMap = true;
            loadSelectedMap();
            inMapSelection = false;
        }
        else if (input == "m" || input == "M" || input == "ь" || input == "Ь") {
            // В меню
            inMapSelection = false;
            showMenu();
            return;
        }
        else if (input == "q" || input == "Q" || input == "й" || input == "Й") {
            // Выход
            running = false;
            inMapSelection = false;
        }
    }
}

void GameController::loadSelectedMap() {
    if (mapManager.isValidMapIndex(currentMapIndex)) {
        const MapInfo& map = mapManager.getMap(currentMapIndex);
        
        selectedMap = map;
        useCustomMap = true;
        
        int currentLevel = model.getCurrentLevel();
        PlayerTank* oldPlayer = model.getPlayer();
        int savedScore = oldPlayer ? oldPlayer->getScore() : 0;
        int savedLives = oldPlayer ? oldPlayer->getLives() : 3;
        
        model = GameWorld(selectedMap.width, selectedMap.height);
        
        model.setCurrentLevel(currentLevel);
        PlayerTank* newPlayer = model.getPlayer();
        if (newPlayer && oldPlayer) {
            newPlayer->setScore(savedScore);
            newPlayer->setLives(savedLives);
            newPlayer->setHealth(3);
        }
        
        mapManager.createWorldFromMap(model, selectedMap);
        model.setState(GameState::PLAYING);
    } else {
        std::cout << "Error loading map, using random generation" << std::endl;
        useCustomMap = false;
        model.loadLevel(model.getCurrentLevel());
    }
}

void GameController::runGame() {
    if (!view.checkTerminalSize()) {
        int requiredWidth = model.getWidth();
        int requiredHeight = model.getHeight();
        
        PlatformUtils::showResizeMessage(requiredWidth, requiredHeight);
        return;
    }

    showMenu();
    while (running) {
        processGameTurn(); 
    }
    
    view.clearScreen();
    std::cout << "Game completed. Thank you for playing!" << std::endl;
}

void GameController::pauseGame() {
    if (model.getState() == GameState::PLAYING) {
        model.setState(GameState::PAUSED);
        
        bool paused = true;
        while (paused && running) {
            handleTerminalResize();
            if (!running) return;

            view.clearScreen();
            if (!view.drawPauseScreen()) {
                continue;
            }
            std::cout.flush();
            
            Command cmd = inputHandler.waitForCommand();
            if (cmd == Command::PAUSE || cmd == Command::CONFIRM) {
                model.setState(GameState::PLAYING);
                paused = false;
            } else if (cmd == Command::EXIT) {
                running = false;
                paused = false;
            } else if (cmd == Command::MENU) {
                if (model.getPlayer() && model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
                }
                // Сбрасываем флаг при возврате в меню
                model.getPlayer()->setScore(0);
                model.getPlayer()->setLives(3);
                showMenu();
                paused = false;
            }
        }
    }
}

void GameController::processGameTurn() {
    handleTerminalResize();
    if (!running) return;

    view.clearScreen();
    int score = 0;
    
    switch (model.getState()) {
        case GameState::PLAYING:
            if (!view.render(model)) {
                // Если рендер не удался из-за размера терминала, выходим из цикла
                return;
            }
            break;
            
        case GameState::LEVEL_COMPLETE:
            showLevelCompleteScreen();
            return;
            
        case GameState::PAUSED:
            view.drawPauseScreen();
            break;
            
        case GameState::GAME_OVER:
            // Сохраняем рекорд только один раз при Game Over
            if (model.getPlayer() && !scoreSaved) {
                score = model.getPlayer()->getScore();
                if (score > 0) {
                    scoreManager.addScore(score);
                    scoreSaved = true; // Помечаем как сохраненный
                }
            }
            view.drawGameOver(model.getPlayer() ? model.getPlayer()->getScore() : 0);
            break;

        default:
            break;
    }
    
    std::cout.flush();
    
    if (model.getState() == GameState::LEVEL_COMPLETE) {
        return;
    }
    
    Command cmd = inputHandler.waitForCommand();
    processCommand(cmd);
    
    if (model.getState() == GameState::PLAYING && 
        (cmd == Command::MOVE_UP || cmd == Command::MOVE_DOWN || 
         cmd == Command::MOVE_LEFT || cmd == Command::MOVE_RIGHT ||
         cmd == Command::FIRE)) {
        
        model.update();
        
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
                // Сохраняем рекорд при выходе в меню из игры
                if (model.getPlayer() && model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
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
                // Сбрасываем флаг при начале новой игры
                scoreSaved = false;
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
                // Сохраняем рекорд при подтверждении на экране Game Over
                if (model.getPlayer() && model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
                }
                // Сбрасываем флаг для новой игры
                scoreSaved = false;
                useCustomMap = false;
                model.setCurrentLevel(1);
                if (model.getPlayer()) {
                    model.getPlayer()->setScore(0);
                    model.getPlayer()->setLives(3);
                }
                showMapSelection();
                model.setState(GameState::PLAYING);
            } else if (model.getState() == GameState::LEVEL_COMPLETE) {
                // Обработка подтверждения на экране завершения уровня
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
            if (model.getPlayer() && model.getPlayer()->getScore() > 0 && !scoreSaved) {
                scoreManager.addScore(model.getPlayer()->getScore());
                scoreSaved = true;
            }
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
        handleTerminalResize();
        if (!running) return;

        view.clearScreen();
        if (!view.drawMenu()) {
            continue;
        }
        
        std::cout << "===== HIGH SCORES =====\n";
        std::vector<int> highScores = scoreManager.getHighScores();
        if (highScores.empty()) {
            std::cout << "   No high scores yet!\n";
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
                // Сбрасываем флаг при начале новой игры
                scoreSaved = false;
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
        handleTerminalResize();
        if (!running) return;

        view.clearScreen();
        if (!view.drawSettings()) {
            continue;
        }

        bool advancedGraphics = settingsManager.getBoolSetting("advanced_graphics", true);
        
        std::cout << "   Graphics: [" << (advancedGraphics ? "X" : " ") << "] Advanced\n";
        std::cout << "            [" << (!advancedGraphics ? "X" : " ") << "] Symbolic\n\n";
        std::cout << "   [F] - Toggle graphics mode\n";
        std::cout << "---------------------------------\n";
        std::cout << "   [ESC] - Back\n";
        std::cout << "   [Q] - Exit\n";
        std::cout << "---------------------------------\n\n";
        std::cout << "   Status: ";
        if (advancedGraphics) {
            std::cout << "Advanced graphics (Unicode)";
            if (!PlatformUtils::supportsUnicode()) {
                std::cout << " - YOUR TERMINAL MAY NOT SUPPORT IT!";
            }
        } else {
            std::cout << "Symbolic graphics (ASCII)";
        }
        std::cout << "\n";
        
        std::cout << "\n=================================\n";
        std::cout << "  Settings are saved automatically\n";
        std::cout << "=================================\n";
        
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
            case Command::FIRE:
                toggleAdvancedGraphics();
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
        loadSelectedMap();
    } else {
        model.loadLevel(nextLevel);
    }
    
    model.setState(GameState::PLAYING);
}

void GameController::showLevelCompleteScreen() {
    if (!model.getPlayer()) return;
    
    // Сохраняем статистику перед переходом
    int currentScore = model.getPlayer()->getScore();
    int currentLives = model.getPlayer()->getLives();
    int currentLevel = model.getCurrentLevel();
    
    // Рассчитываем бонусы
    int levelBonus = currentLevel * 100;
    int livesBonus = currentLives * 25;
    int totalBonus = levelBonus + livesBonus;
    
    // Начисляем бонусы
    model.getPlayer()->addScore(totalBonus);
    
    bool inLevelCompleteScreen = true;
    
    while (inLevelCompleteScreen && running) {
        handleTerminalResize();
        if (!running) return;

        view.clearScreen();
        if (!view.drawLevelComplete(currentScore, currentLevel, currentLives)) {
            continue;
        }
        std::cout.flush();
        
        Command cmd = inputHandler.waitForCommand();
        switch (cmd) {
            case Command::CONFIRM:
                // Сохраняем рекорд при переходе на следующий уровень через меню
                if (cmd == Command::MENU && model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
                }
                model.setCurrentLevel(currentLevel + 1);
                loadNextLevel();
                inLevelCompleteScreen = false;
                break;
                
            case Command::MENU:
                // Сохраняем рекорд при выходе в меню с экрана завершения уровня
                if (model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
                }
                showMenu();
                inLevelCompleteScreen = false;
                break;
                
            case Command::EXIT:
                // Сохраняем рекорд при выходе из игры с экрана завершения уровня
                if (model.getPlayer()->getScore() > 0 && !scoreSaved) {
                    scoreManager.addScore(model.getPlayer()->getScore());
                    scoreSaved = true;
                }
                running = false;
                inLevelCompleteScreen = false;
                break;
                
            default:
                break;
        }
    }
}
