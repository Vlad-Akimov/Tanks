#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <vector>
#include <memory>
#include "GameObject.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Obstacle.h"

class Projectile;

class GameWorld {
private:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    PlayerTank* player;
    int currentLevel;
    bool gameOver;
    bool levelCompleted;

public:
    GameWorld();
    ~GameWorld() = default;
    
    // Управление игровым миром
    void initializeLevel(int level);
    void update();
    void checkCollisions();
    void spawnEnemies(int count);
    void spawnObstacles();
    void cleanupDestroyedObjects();
    
    // Геттеры
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const;
    PlayerTank* getPlayer() const;
    bool isGameOver() const;
    bool isLevelCompleted() const;
    int getCurrentLevel() const;
};

#endif // GAMEWORLD_H