/**
 * @file Projectile.h
 * @author Vld251
 * @brief Projectile class for bullets fired by tanks with collision detection.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <vector>

class Tank;

/**
 * @brief Class representing a projectile fired by tanks.
 * 
 * Represents bullets or shells that travel across the game field,
 * dealing damage to objects they collide with.
 */
class Projectile : public GameObject {
private:
    int damage;         ///< Damage dealt by projectile
    Tank* owner;        ///< Tank that fired this projectile
    bool processed;     ///< Whether projectile has been processed for collisions

public:
    /**
     * @brief Constructs a Projectile object.
     * @param pos Initial position of the projectile.
     * @param dir Direction of travel.
     * @param dmg Damage dealt by projectile.
     * @param ownerTank Tank that fired this projectile.
     * @returns None
     */
    Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank);
    
    /**
     * @brief Gets the owner tank of this projectile.
     * @return Pointer to owner tank.
     */
    Tank* getOwner() const;
    
    /**
     * @brief Gets the damage value of this projectile.
     * @return Damage value.
     */
    int getDamage() const;
    
    /**
     * @brief Checks if projectile has been processed.
     * @return true if processed, false otherwise.
     */
    bool isProcessed() const;
    
    /**
     * @brief Marks projectile as processed.
     * @returns None
     */
    void markProcessed();

    /**
     * @brief Sets new damage value for projectile.
     * @param newDamage New damage value.
     * @returns None
     */
    void setDamage(int newDamage);
    
    /**
     * @brief Updates projectile state (travels and self-destructs).
     * @returns None
     */
    void update() override;
    
    /**
     * @brief Gets the bounding dimensions of the projectile.
     * @return Point representing projectile's width and height.
     */
    Point getBounds() const override;
    
    /**
     * @brief Gets the character symbol representing the projectile.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
    
    /**
     * @brief Calculates full trajectory of projectile.
     * @param maxRange Maximum range to calculate (default: 20).
     * @return Vector of points along trajectory.
     */
    std::vector<Point> getTrajectory(int maxRange = 20) const;
};

#endif // PROJECTILE_H
