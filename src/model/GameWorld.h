#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <vector>
#include <memory>
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
    GameState state;
    int currentLevel;
    int fieldWidth, fieldHeight;
    PlayerTank* player;

public:
    GameWorld(int width, int height);
    
    // Основные методы игры
    void update();
    void loadLevel(int level);
    void checkCollisions();
    void spawnBonus();
    std::vector<GameObject*> getObjectsInRadius(Point center, int radius) const;
    
    // Управление состоянием игры
    void setState(GameState newState);
    GameState getState() const { return state; }
    
    // Геттеры
    int getWidth() const { return fieldWidth; }
    int getHeight() const { return fieldHeight; }
    PlayerTank* getPlayer() const { return player; }
    const auto& getObjects() const { return objects; }
    const auto& getProjectiles() const { return projectiles; }
    
    // Добавление объектов
    void addObject(std::unique_ptr<GameObject> obj);
    void addProjectile(std::unique_ptr<Projectile> proj);
    void addBonus(std::unique_ptr<Bonus> bonus);
};

#endif // GAMEWORLD_H