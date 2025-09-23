// src/model/GameModel.h
#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <vector>
#include <memory>
#include "GameObject.h"
#include "Tank.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Obstacle.h"
#include "Projectile.h"
#include "Bonus.h"
#include "../utils/Constants.h"

class LevelLoader;

class GameModel {
private:
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<std::unique_ptr<Bonus>> bonuses;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    PlayerTank* playerTank;
    Constants::GameState state;
    int currentLevel;
    int score;
    int lives;
    int levelWidth, levelHeight;
    
    // Private methods
    void cleanUpDestroyedObjects();
    void spawnRandomBonus();

public:
    GameModel();
    ~GameModel();
    
    void update();
    bool loadLevel(int level);
    void checkCollisions();
    void spawnBonus(int x, int y, Constants::BonusType type);
    std::vector<GameObject*> getObjectsInRadius(int x, int y, int radius) const;
    void addProjectile(Projectile* projectile);
    void addBonus(Bonus* bonus);
    
    // Getters
    Constants::GameState getState() const { return state; }
    int getCurrentLevel() const { return currentLevel; }
    int getScore() const { return score; }
    int getLives() const { return lives; }
    PlayerTank* getPlayerTank() const { return playerTank; }
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const { return objects; }
    const std::vector<std::unique_ptr<Bonus>>& getBonuses() const { return bonuses; }
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return projectiles; }
    int getLevelWidth() const { return levelWidth; }
    int getLevelHeight() const { return levelHeight; }
    
    // Setters
    void setState(Constants::GameState newState) { state = newState; }
    void setScore(int newScore) { score = newScore; }
    void setLives(int newLives) { lives = newLives; }
};

#endif