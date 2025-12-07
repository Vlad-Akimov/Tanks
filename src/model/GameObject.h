/**
 * @file GameObject.h
 * @author Vld251
 * @brief Base class and structures for all game objects with position, direction, and health.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

/**
 * @brief Structure representing a 2D point with integer coordinates.
 */
struct Point {
    int x, y;
    
    /**
     * @brief Constructs a Point object.
     * @param x The x-coordinate (default: 0).
     * @param y The y-coordinate (default: 0).
     * @returns None
     */
    Point(int x = 0, int y = 0);
    
    /**
     * @brief Checks if two points are equal.
     * @param other The other point to compare with.
     * @return true if points are equal, false otherwise.
     */
    bool operator==(const Point& other) const;
    
    /**
     * @brief Checks if two points are not equal.
     * @param other The other point to compare with.
     * @return true if points are not equal, false otherwise.
     */
    bool operator!=(const Point& other) const;
};

/**
 * @brief Enumeration representing possible movement directions.
 */
enum class Direction { UP, DOWN, LEFT, RIGHT };

/**
 * @brief Abstract base class for all game objects.
 * 
 * Represents a game object with position, direction, speed, health, 
 * and destructibility properties. All game entities inherit from this class.
 */
class GameObject {
protected:
    Point position;         ///< Current position of the object
    Direction direction;    ///< Current facing direction
    int speed;             ///< Movement speed
    int health;            ///< Current health points
    bool destructible;     ///< Whether object can be destroyed

public:
    /**
     * @brief Constructs a GameObject.
     * @param pos Initial position of the object.
     * @param dir Initial direction of the object.
     * @param spd Movement speed of the object.
     * @param hp Initial health points.
     * @param destruct Whether object is destructible.
     * @returns None
     */
    GameObject(Point pos, Direction dir, int spd, int hp, bool destruct);
    
    /**
     * @brief Virtual destructor for proper cleanup.
     */
    virtual ~GameObject() = default;
    
    /**
     * @brief Moves the object in specified direction.
     * @param dir Direction to move in.
     * @returns None
     */
    virtual void move(Direction dir);
    
    /**
     * @brief Rotates the object to face new direction.
     * @param newDirection New direction to face.
     * @returns None
     */
    void rotate(Direction newDirection);
    
    /**
     * @brief Applies damage to the object.
     * @param damage Amount of damage to apply.
     * @returns None
     */
    virtual void takeDamage(int damage);
    
    /**
     * @brief Checks if object is destroyed.
     * @return true if object is destroyed, false otherwise.
     */
    bool isDestroyed() const;
    
    /**
     * @brief Sets destroyed state of the object.
     * @param destroyed Whether object should be marked as destroyed.
     * @returns None
     */
    void setDestroyed(bool destroyed);
    
    /**
     * @brief Gets the bounding dimensions of the object.
     * @return Point representing object's width and height.
     */
    virtual Point getBounds() const = 0;
    
    /**
     * @brief Updates object state (called each game tick).
     * @returns None
     */
    virtual void update() = 0;
    
    /**
     * @brief Gets the character symbol representing the object.
     * @return Character symbol for display.
     */
    virtual char getSymbol() const = 0;
    
    /**
     * @brief Gets current position of the object.
     * @return Current position as Point.
     */
    Point getPosition() const;
    
    /**
     * @brief Gets current direction of the object.
     * @return Current direction.
     */
    Direction getDirection() const;
    
    /**
     * @brief Gets movement speed of the object.
     * @return Movement speed value.
     */
    int getSpeed() const;
    
    /**
     * @brief Gets current health points.
     * @return Current health value.
     */
    int getHealth() const;
    
    /**
     * @brief Checks if object is destructible.
     * @return true if object is destructible, false otherwise.
     */
    bool isDestructible() const;
    
    /**
     * @brief Sets new position for the object.
     * @param newPos New position to set.
     * @returns None
     */
    void setPosition(Point newPos);
    
    /**
     * @brief Sets new health value for the object.
     * @param newHealth New health value to set.
     * @returns None
     */
    void setHealth(int newHealth);
};

#endif // GAMEOBJECT_H
