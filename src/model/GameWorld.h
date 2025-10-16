#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include "GameObject.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Obstacle.h"
#include "Bonus.h"
#include "Projectile.h"

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, SETTINGS };

class GameWorld {
private:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<std::unique_ptr<Bonus>> bonuses;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    int fieldWidth, fieldHeight;
    GameState state;
    int currentLevel;
    PlayerTank* player;
    int enemyCount;
    int maxEnemies;

    struct DifficultyParams {
        int brickCount;
        int steelCount;
        int waterCount;
        int forestCount;
        int brickClusters;
        int steelClusters;
        int waterClusters;
        int forestClusters;
        bool organicClusters;
    };
    
    // Приватные методы для обработки столкновений
    void checkProjectileCollisions();
    void checkBonusCollisions();
    void checkTankObstacleCollisions();
    void checkTankTankCollisions();
    void checkBoundaryCollisions();
    bool checkPointCollision(const Point& point, GameObject* excludeObj = nullptr);
    void handleTankTankCollision(Tank* tank1);
    void handleTankObstacleCollision(Tank* tank);
    bool handleProjectileHit(GameObject* target, Projectile* projectile, int damage);
    
    DifficultyParams adjustDifficulty(int level);
    bool isValidPosition(const Point& pos, const Point& bounds, const GameObject* excludeObj = nullptr) const;
    void createLevelObstacles(int level);
    void createEnemies(int level);
    void updateEnemyAI();
    void updateEnemyMovement(EnemyTank* enemy);
    void handleEnemyCollision(EnemyTank* enemy, Direction moveDir);
    void cleanupDestroyedObjects();
    void checkGameConditions();

public:
    GameWorld(int width, int height);
    
    void update();
    void loadLevel(int level);
    void checkCollisions();
    void spawnBonus();
    bool isValidBonusPosition(const Point& pos) const;
    bool isPositionAccessible(const Point& pos) const;
    std::vector<GameObject*> getObjectsInRadius(Point center, int radius) const;
    
    void setState(GameState newState);
    GameState getState() const;
    
    int getWidth() const;
    int getHeight() const;
    PlayerTank* getPlayer() const;
    void setPlayer(PlayerTank* newPlayer);

    const std::vector<std::unique_ptr<GameObject>>& getObjects() const;
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const;
    const std::vector<std::unique_ptr<Bonus>>& getBonuses() const;

    int getCurrentLevel() const;
    
    void addObject(std::unique_ptr<GameObject> obj);
    void addProjectile(std::unique_ptr<Projectile> proj);
    void addBonus(std::unique_ptr<Bonus> bonus);
    
    void playerFire();
    void playerMove(Direction dir);
};

#endif // GAMEWORLD_H
