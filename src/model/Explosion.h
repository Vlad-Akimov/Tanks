/**
 * @file Explosion.h
 * @author Vld251
 * @brief Visual explosion effects for game events like tank destruction.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"

/**
 * @brief Class representing an explosion effect.
 * 
 * Visual effect displayed when objects are destroyed.
 * Has limited lifetime and disappears automatically.
 */
class Explosion : public GameObject {
private:
    int lifetime;  ///< Remaining lifetime of explosion

public:
    /**
     * @brief Constructs an Explosion object.
     * @param pos Position of the explosion.
     * @returns None
     */
    Explosion(Point pos);
    
    /**
     * @brief Updates explosion state (decrements lifetime).
     * @returns None
     */
    void update() override;
    
    /**
     * @brief Gets the bounding dimensions of the explosion.
     * @return Point representing explosion's width and height.
     */
    Point getBounds() const override;
    
    /**
     * @brief Gets the character symbol representing the explosion.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
};

#endif // EXPLOSION_H
