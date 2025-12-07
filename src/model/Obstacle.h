/**
 * @file Obstacle.h
 * @author Vld251
 * @brief Environmental obstacles including walls, water, forests, and their properties.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

/**
 * @brief Enumeration representing obstacle types.
 */
enum class ObstacleType { BRICK, STEEL, WATER, FOREST };

/**
 * @brief Base class for all environmental obstacles.
 * 
 * Represents static objects in the game world that can block movement,
 * provide cover, or have other environmental effects.
 */
class Obstacle : public GameObject {
private:
    ObstacleType type;    ///< Type of obstacle
    bool movable;         ///< Whether obstacle can be moved

public:
    /**
     * @brief Constructs an Obstacle object.
     * @param pos Position of the obstacle.
     * @param obsType Type of obstacle.
     * @param movable Whether obstacle can be moved (default: false).
     * @returns None
     */
    Obstacle(Point pos, ObstacleType obsType, bool movable = false);
    
    /**
     * @brief Gets the type of obstacle.
     * @return Obstacle type.
     */
    ObstacleType getType() const;
    
    /**
     * @brief Checks if obstacle is movable.
     * @return true if movable, false otherwise.
     */
    bool isMovable() const;
    
    /**
     * @brief Checks if obstacle is destructible.
     * @return true if destructible, false otherwise.
     */
    bool isDestructible() const;
    
    /**
     * @brief Checks if obstacle is passable for tanks.
     * @return true if passable, false otherwise.
     */
    bool isPassable() const;
    
    /**
     * @brief Checks if obstacle is passable for projectiles.
     * @return true if projectiles can pass through, false otherwise.
     */
    bool isProjectilePassable() const;
    
    /**
     * @brief Checks if obstacle is transparent to projectiles.
     * @return true if transparent to projectiles, false otherwise.
     */
    bool isProjectileTransparent() const;
    
    /**
     * @brief Checks if obstacle provides cover for tanks.
     * @return true if provides cover, false otherwise.
     */
    bool providesCover() const;
    
    /**
     * @brief Updates obstacle state.
     * @returns None
     */
    void update() override;
    
    /**
     * @brief Gets the bounding dimensions of the obstacle.
     * @return Point representing obstacle's width and height.
     */
    Point getBounds() const override;
    
    /**
     * @brief Gets the character symbol representing the obstacle.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
    
    /**
     * @brief Applies damage to the obstacle.
     * @param damage Amount of damage to apply.
     * @returns None
     */
    void takeDamage(int damage) override;
};

/**
 * @brief Class representing a steel wall obstacle.
 */
class SteelWall : public Obstacle {
public:
    /**
     * @brief Constructs a SteelWall object.
     * @param pos Position of the steel wall.
     * @returns None
     */
    SteelWall(Point pos);
    
    /**
     * @brief Gets the character symbol for steel wall.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
    
    /**
     * @brief Applies damage to steel wall.
     * @param damage Amount of damage to apply.
     * @returns None
     */
    void takeDamage(int damage) override;
};

/**
 * @brief Class representing a water obstacle.
 */
class Water : public Obstacle {
public:
    /**
     * @brief Constructs a Water object.
     * @param pos Position of the water.
     * @returns None
     */
    Water(Point pos);
    
    /**
     * @brief Gets the character symbol for water.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
};

/**
 * @brief Class representing a forest obstacle.
 */
class Forest : public Obstacle {
public:
    /**
     * @brief Constructs a Forest object.
     * @param pos Position of the forest.
     * @returns None
     */
    Forest(Point pos);
    
    /**
     * @brief Gets the character symbol for forest.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
};

#endif // OBSTACLE_H
