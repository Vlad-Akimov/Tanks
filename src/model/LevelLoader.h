// src/model/LevelLoader.h
#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <string>
#include <vector>
#include "GameObject.h"
#include "Tank.h"
#include "Obstacle.h"
#include "../utils/Constants.h"

class GameModel;

class LevelLoader {
public:
    static bool loadLevelFromFile(GameModel* model, const std::string& filename, int levelNumber);
    static bool loadLevelFromString(GameModel* model, const std::string& levelData, int levelNumber);
    
private:
    static GameObject* createGameObjectFromChar(char ch, int x, int y);
    static Constants::Direction charToDirection(char dirChar);
};

#endif