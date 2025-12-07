/**
 * @file Bonus.h
 * @author Vld251
 * @brief Power-up bonuses with various effects for enhancing player abilities.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef BONUS_H
#define BONUS_H

#include "GameObject.h"

class PlayerTank;

enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };

/**
 * Represents a bonus object in the game.
 */
class Bonus : public GameObject {
private:
    BonusType type;
    int duration;
    bool active;
    int activationTime;

public:
    /**
     * Represents a bonus at a specific position with a given type and duration.
     *
     * @param pos The position of the bonus.
     * @param bonusType The type of the bonus.
     * @param dur The duration of the bonus (default is 0).
     *
     * @returns None
     */
    Bonus(Point pos, BonusType bonusType, int dur = 0);

    /**
     * Applies an effect to the specified PlayerTank.
     *
     * @param playerTank A pointer to the PlayerTank object to apply the effect to.
     *
     * @returns None
     */
    void applyEffect(PlayerTank* playerTank);

    /**
     * Deactivates a specific functionality or operation.
     *
     * @returns None
     */
    void deactivate();

    /**
     * Check if the object is active.
     *
     * @return true if the object is active, false otherwise.
     */
    bool isActive() const;

    /**
     * Checks if the projectile is passable.
     *
     * @return true if the projectile is passable, false otherwise.
     */
    bool isProjectilePassable() const;

    /**
     * Retrieves the type of bonus.
     *
     * @return The type of bonus as a BonusType enum value.
     */
    BonusType getType() const;
    
    /**
     * Updates the object according to its internal state.
     *
     * This function overrides a base class method and updates the object based on its internal state.
     *
     * @returns None
     */
    void update() override;

    /**
     * Returns the bounds of the Point object.
     *
     * @return Point - The bounds of the Point object.
     */
    Point getBounds() const override;

    /**
     * Get the symbol representation of the bonus type.
     *
     * @return The symbol representing the bonus type.
     */
    char getSymbol() const override;
};

#endif // BONUS_H
