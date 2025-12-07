/**
 * @file GameWorld.h
 * @author Vld251
 * @brief Main game world manager handling objects, collisions, states, and game logic.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

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
#include "Explosion.h"

/**
 * @brief Enumeration representing possible game states.
 */
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, SETTINGS, LEVEL_COMPLETE };

/**
 * @brief Main game world class managing all game objects and logic.
 * 
 * Responsible for object management, collision detection, level generation,
 * game state management, and overall game simulation.
 */
class GameWorld {
private:
    std::vector<std::unique_ptr<GameObject>> objects;   ///< All game objects
    std::vector<std::unique_ptr<Bonus>> bonuses;        ///< Active bonuses
    std::vector<std::unique_ptr<Projectile>> projectiles; ///< Active projectiles
    std::vector<std::unique_ptr<Explosion>> explosions; ///< Active explosions

    int fieldWidth, fieldHeight;    ///< Dimensions of game field
    GameState state;                ///< Current game state
    int currentLevel;               ///< Current level number
    PlayerTank* player;             ///< Pointer to player tank
    int enemyCount;                 ///< Current number of enemies
    int maxEnemies;                 ///< Maximum enemies for current level
    int damageFlashCounter;         ///< Counter for damage flash effect

    /**
     * @brief Structure containing difficulty parameters for level generation.
     */
    struct DifficultyParams {
        int brickCount;        ///< Number of brick obstacles
        int steelCount;        ///< Number of steel obstacles
        int waterCount;        ///< Number of water obstacles
        int forestCount;       ///< Number of forest obstacles
        int brickClusters;     ///< Number of brick clusters
        int steelClusters;     ///< Number of steel clusters
        int waterClusters;     ///< Number of water clusters
        int forestClusters;    ///< Number of forest clusters
        bool organicClusters;  ///< Whether to use organic cluster generation
    };
    
    void checkProjectileCollisions();       ///< Checks projectile collisions
    void checkBonusCollisions();           ///< Checks bonus collection
    void checkTankObstacleCollisions();    ///< Checks tank-obstacle collisions
    void checkTankTankCollisions();        ///< Checks tank-tank collisions
    void checkBoundaryCollisions();        ///< Checks boundary collisions
    bool checkPointCollision(const Point& point, GameObject* excludeObj = nullptr); ///< Checks collision at point
    void handleTankTankCollision(Tank* tank1); ///< Handles tank-tank collision
    void handleTankObstacleCollision(Tank* tank); ///< Handles tank-obstacle collision
    bool handleProjectileHit(GameObject* target, Projectile* projectile, int damage); ///< Handles projectile hit
    void applySlowToEnemies(int duration); ///< Applies slow effect to all enemies
    
    DifficultyParams adjustDifficulty(int level); ///< Adjusts difficulty based on level
    bool isValidPosition(const Point& pos, const Point& bounds, const GameObject* excludeObj = nullptr) const; ///< Checks if position is valid
    void createLevelObstacles(int level);  ///< Creates obstacles for level
    void createEnemies(int level);         ///< Creates enemies for level
    void updateEnemyAI();                  ///< Updates AI for all enemies
    void updateEnemyMovement(EnemyTank* enemy); ///< Updates movement for specific enemy
    void handleEnemyCollision(EnemyTank* enemy, Direction moveDir); ///< Handles enemy collision
    void cleanupDestroyedObjects();        ///< Removes destroyed objects
    void checkGameConditions();            ///< Checks win/lose conditions

public:
    /**
     * @brief Constructs a GameWorld object.
     * @param width Width of game field.
     * @param height Height of game field.
     * @returns None
     */
    GameWorld(int width, int height);
    
    /**
     * @brief Updates all game objects and logic.
     * @returns None
     */
    void update();
    
    /**
     * @brief Loads specified level.
     * @param level Level number to load.
     * @returns None
     */
    void loadLevel(int level);
    
    /**
     * @brief Checks all types of collisions.
     * @returns None
     */
    void checkCollisions();
    
    /**
     * @brief Spawns a bonus at random location.
     * @returns None
     */
    void spawnBonus();
    
    /**
     * @brief Checks if position is valid for bonus placement.
     * @param pos Position to check.
     * @return true if position is valid for bonus, false otherwise.
     */
    bool isValidBonusPosition(const Point& pos) const;
    
    /**
     * @brief Checks if position is accessible for tanks.
     * @param pos Position to check.
     * @return true if position is accessible, false otherwise.
     */
    bool isPositionAccessible(const Point& pos) const;
    
    /**
     * @brief Gets all objects within specified radius.
     * @param center Center point for radius.
     * @param radius Search radius.
     * @return Vector of pointers to objects within radius.
     */
    std::vector<GameObject*> getObjectsInRadius(Point center, int radius) const;
    
    /**
     * @brief Sets current game state.
     * @param newState New game state.
     * @returns None
     */
    void setState(GameState newState);
    
    /**
     * @brief Gets current game state.
     * @return Current game state.
     */
    GameState getState() const;
    
    /**
     * @brief Gets width of game field.
     * @return Field width.
     */
    int getWidth() const;
    
    /**
     * @brief Gets height of game field.
     * @return Field height.
     */
    int getHeight() const;
    
    /**
     * @brief Gets current enemy count.
     * @return Number of active enemies.
     */
    int getEnemyCount() const;
    
    /**
     * @brief Sets current level number.
     * @param level Level number to set.
     * @returns None
     */
    void setCurrentLevel(int level);
    
    /**
     * @brief Gets pointer to player tank.
     * @return Pointer to player tank.
     */
    PlayerTank* getPlayer() const;
    
    /**
     * @brief Sets player tank pointer.
     * @param newPlayer Pointer to new player tank.
     * @returns None
     */
    void setPlayer(PlayerTank* newPlayer);

    /**
     * @brief Gets all game objects.
     * @return Const reference to objects vector.
     */
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const;
    
    /**
     * @brief Gets all active projectiles.
     * @return Const reference to projectiles vector.
     */
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const;
    
    /**
     * @brief Gets all active bonuses.
     * @return Const reference to bonuses vector.
     */
    const std::vector<std::unique_ptr<Bonus>>& getBonuses() const;

    /**
     * @brief Gets current level number.
     * @return Current level.
     */
    int getCurrentLevel() const;
    
    /**
     * @brief Adds object to game world.
     * @param obj Unique pointer to object to add.
     * @returns None
     */
    void addObject(std::unique_ptr<GameObject> obj);
    
    /**
     * @brief Adds projectile to game world.
     * @param proj Unique pointer to projectile to add.
     * @returns None
     */
    void addProjectile(std::unique_ptr<Projectile> proj);
    
    /**
     * @brief Adds bonus to game world.
     * @param bonus Unique pointer to bonus to add.
     * @returns None
     */
    void addBonus(std::unique_ptr<Bonus> bonus);
    
    /**
     * @brief Triggers player tank to fire.
     * @returns None
     */
    void playerFire();
    
    /**
     * @brief Moves player tank in specified direction.
     * @param dir Direction to move.
     * @returns None
     */
    void playerMove(Direction dir);

    /**
     * @brief Adds explosion to game world.
     * @param explosion Unique pointer to explosion to add.
     * @returns None
     */
    void addExplosion(std::unique_ptr<Explosion> explosion);
    
    /**
     * @brief Gets all active explosions.
     * @return Const reference to explosions vector.
     */
    const std::vector<std::unique_ptr<Explosion>>& getExplosions() const;

    /**
     * @brief Triggers damage flash effect.
     * @returns None
     */
    void triggerDamageFlash();
    
    /**
     * @brief Checks if damage flash effect is active.
     * @return true if damage flash is active, false otherwise.
     */
    bool isDamageFlashActive() const;
};

#endif // GAMEWORLD_H
