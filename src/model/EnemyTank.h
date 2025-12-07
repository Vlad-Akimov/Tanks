/**
 * @file EnemyTank.h
 * @author Vld251
 * @brief AI-controlled enemy tanks with various behaviors, types, and difficulty levels.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "Tank.h"
#include <stdlib.h>

/**
 * @brief Enumeration representing AI behavior patterns.
 */
enum class AIBehavior { RANDOM, AGGRESSIVE, DEFENSIVE };

/**
 * @brief Enumeration representing enemy tank types.
 */
enum class EnemyTankType {
    BASIC,      ///< Standard enemy tank
    FAST,       ///< Fast-moving tank with low health
    DAMAGE,     ///< Tank with increased damage output
    ARMORED     ///< Tank with increased health
};

/**
 * @brief Class representing enemy tanks with AI behavior.
 * 
 * Inherits from Tank and adds AI-controlled movement, different behaviors,
 * and various tank types with unique characteristics.
 */
class EnemyTank : public Tank {
private:
    AIBehavior behavior;             ///< Current AI behavior pattern
    int difficulty;                  ///< AI difficulty level
    EnemyTankType tankType;          ///< Type of enemy tank
    Point playerLastPosition;        ///< Last known player position
    int moveCooldown;                ///< Cooldown timer for movement
    
    void decideNextMove();           ///< Decides next action based on behavior
    void randomBehavior();           ///< Executes random movement behavior
    void aggressiveBehavior();       ///< Executes aggressive behavior
    void defensiveBehavior();        ///< Executes defensive behavior
    void moveTowardsPlayer();        ///< Moves towards player position
    void defensiveMove();            ///< Moves away from player
    void cautiousApproach();         ///< Carefully approaches player
    void findCover();                ///< Searches for cover
    void attemptShot();              ///< Attempts to shoot at player
    Direction getOppositeDirection(Direction dir) const;   ///< Gets opposite direction
    Direction getPerpendicularDirection(Direction dir) const;  ///< Gets perpendicular direction
    bool hasClearShot() const;       ///< Checks if tank has clear line of sight to player

public:
    /**
     * @brief Constructs an EnemyTank object.
     * @param pos Initial position of the enemy tank.
     * @param behav AI behavior pattern.
     * @param diff Difficulty level.
     * @param type Type of enemy tank (default: BASIC).
     * @returns None
     */
    EnemyTank(Point pos, AIBehavior behav, int diff, EnemyTankType type = EnemyTankType::BASIC);
    
    /**
     * @brief Updates last known player position.
     * @param playerPos Current player position.
     * @returns None
     */
    void setPlayerPosition(Point playerPos);
    
    /**
     * @brief Updates enemy tank state and AI decisions.
     * @returns None
     */
    void update() override;
    
    /**
     * @brief Moves enemy tank in specified direction.
     * @param dir Direction to move in.
     * @returns None
     */
    void move(Direction dir) override;
    
    /**
     * @brief Gets the bounding dimensions of the enemy tank.
     * @return Point representing tank's width and height.
     */
    Point getBounds() const override;
    
    /**
     * @brief Gets the character symbol representing the enemy tank.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
    
    /**
     * @brief Checks if enemy can see the player.
     * @return true if player is visible, false otherwise.
     */
    bool canSeePlayer() const;
    
    /**
     * @brief Gets direction towards last known player position.
     * @return Direction to player.
     */
    Direction getDirectionToPlayer() const;
    
    /**
     * @brief Gets Manhattan distance to player.
     * @return Distance to player, or -1 if player not known.
     */
    int getDistanceToPlayer() const;
    
    /**
     * @brief Fires projectile from enemy tank.
     * @return Pointer to created projectile.
     */
    Projectile* fire() override;

    /**
     * @brief Gets the type of enemy tank.
     * @return Enemy tank type.
     */
    EnemyTankType getTankType() const;

    /**
     * @brief Applies slow effect to enemy tank.
     * @param duration Duration of slow effect.
     * @returns None
     */
    void applySlowEffect(int duration);
};

#endif // ENEMYTANK_H
