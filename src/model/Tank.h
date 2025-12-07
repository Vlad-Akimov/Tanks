/**
 * @file Tank.h
 * @author Vld251
 * @brief Abstract tank base class with shooting, bonuses, and combat mechanics.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef TANK_H
#define TANK_H

#include "GameObject.h"
#include "Projectile.h"
#include "Bonus.h"

/**
 * @brief Abstract base class for all tank entities.
 * 
 * Represents a tank with shooting capabilities, bonus effects,
 * and various combat-related properties.
 */
class Tank : public GameObject {
protected:
    int fireRate;              ///< Base fire rate (shots per second equivalent)
    int reloadTime;            ///< Time required to reload
    int currentReload;         ///< Current reload timer
    bool hasShield;           ///< Whether tank has active shield
    bool doubleFire;          ///< Whether tank has double fire enabled
    int speedBoost;           ///< Additional speed from boost

    int speedSlow;            ///< Speed reduction from slow effects
    int speedSlowDuration;    ///< Duration of slow effect
    
    int shieldDuration;       ///< Remaining shield duration
    int doubleFireDuration;   ///< Remaining double fire duration
    int speedBoostDuration;   ///< Remaining speed boost duration

public:
    /**
     * @brief Constructs a Tank object.
     * @param pos Initial position of the tank.
     * @param dir Initial direction of the tank.
     * @param spd Movement speed of the tank.
     * @param hp Initial health points.
     * @param fireRate Base fire rate of the tank.
     * @returns None
     */
    Tank(Point pos, Direction dir, int spd, int hp, int fireRate);
    
    /**
     * @brief Virtual destructor for proper cleanup.
     */
    virtual ~Tank() = default;
    
    /**
     * @brief Fires a projectile from the tank.
     * @return Pointer to created projectile, or nullptr if cannot fire.
     */
    virtual Projectile* fire();
    
    /**
     * @brief Applies a bonus effect to the tank.
     * @param bonus Type of bonus to apply.
     * @returns None
     */
    void applyBonus(BonusType bonus);
    
    /**
     * @brief Updates bonus effect durations.
     * @returns None
     */
    void updateBonus();
    
    /**
     * @brief Decrements reload timer.
     * @returns None
     */
    void reload();
    
    /**
     * @brief Checks if tank can fire.
     * @return true if tank can fire, false otherwise.
     */
    bool canFire() const;
    
    /**
     * @brief Applies damage to tank, considering shield.
     * @param damage Amount of damage to apply.
     * @returns None
     */
    void takeDamage(int damage) override;
    
    /**
     * @brief Moves tank in specified direction with bonus speed.
     * @param dir Direction to move in.
     * @returns None
     */
    void move(Direction dir) override;
    
    /**
     * @brief Checks if tank has active shield.
     * @return true if shield is active, false otherwise.
     */
    bool getHasShield() const;
    
    /**
     * @brief Checks if double fire is active.
     * @return true if double fire is active, false otherwise.
     */
    bool getDoubleFire() const;
    
    /**
     * @brief Gets maximum remaining bonus duration.
     * @return Maximum remaining bonus duration.
     */
    int getBonusDuration() const;

    /**
     * @brief Applies slow effect to the tank.
     * @param duration Duration of slow effect.
     * @returns None
     */
    void applySlowEffect(int duration);
    
    /**
     * @brief Gets actual speed considering all modifiers.
     * @return Actual movement speed.
     */
    int getActualSpeed() const;
    
    /**
     * @brief Gets remaining slow effect duration.
     * @return Remaining slow duration.
     */
    int getSpeedSlowDuration() const;
    
    /**
     * @brief Sets reload time for the tank.
     * @param reload New reload time.
     * @returns None
     */
    void setReloadTime(int reload);
    
    /**
     * @brief Sets fire rate for the tank.
     * @param rate New fire rate.
     * @returns None
     */
    void setFireRate(int rate);
    
    /**
     * @brief Sets base speed for the tank.
     * @param newSpeed New base speed.
     * @returns None
     */
    void setSpeed(int newSpeed);
    
    /**
     * @brief Gets remaining shield duration.
     * @return Remaining shield duration.
     */
    int getShieldDuration() const;
    
    /**
     * @brief Gets remaining double fire duration.
     * @return Remaining double fire duration.
     */
    int getDoubleFireDuration() const;
    
    /**
     * @brief Gets remaining speed boost duration.
     * @return Remaining speed boost duration.
     */
    int getSpeedBoostDuration() const;
    
    /**
     * @brief Gets current slow effect strength.
     * @return Current speed reduction value.
     */
    int getSpeedSlow() const;
    
    /**
     * @brief Updates tank state (called each game tick).
     * @returns None
     */
    virtual void update() override = 0;
    
    /**
     * @brief Gets the bounding dimensions of the tank.
     * @return Point representing tank's width and height.
     */
    virtual Point getBounds() const override = 0;
    
    /**
     * @brief Gets the character symbol representing the tank.
     * @return Character symbol for display.
     */
    virtual char getSymbol() const override = 0;
};

#endif // TANK_H
