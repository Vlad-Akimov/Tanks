/**
 * @file PlayerTank.h
 * @author Vld251
 * @brief Player-controlled tank with lives, score system, and special abilities.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Tank.h"

/**
 * @brief Class representing the player-controlled tank.
 * 
 * Inherits from Tank and adds player-specific features like lives, score,
 * and special abilities.
 */
class PlayerTank : public Tank {
private:
    int lives;  ///< Number of remaining lives
    int score;  ///< Current player score

public:
    /**
     * @brief Constructs a PlayerTank object.
     * @param pos Initial position of the player tank.
     * @returns None
     */
    PlayerTank(Point pos);
    
    /**
     * @brief Resets player tank to initial state.
     * @param level Current level number.
     * @returns None
     */
    void reset(int level);
    
    /**
     * @brief Adds points to player's score.
     * @param points Points to add.
     * @returns None
     */
    void addScore(int points);
    
    /**
     * @brief Reduces player's lives by one.
     * @returns None
     */
    void loseLife();
    
    /**
     * @brief Checks if player has remaining lives.
     * @return true if player has lives, false otherwise.
     */
    bool hasLives() const;
    
    /**
     * @brief Gets number of remaining lives.
     * @return Number of lives.
     */
    int getLives() const;
    
    /**
     * @brief Gets current player score.
     * @return Current score.
     */
    int getScore() const;

    /**
     * @brief Sets player score.
     * @param savedScore Score to set.
     * @returns None
     */
    void setScore(int savedScore);
    
    /**
     * @brief Sets number of player lives.
     * @param lvs Number of lives to set.
     * @returns None
     */
    void setLives(int lvs);
    
    /**
     * @brief Updates player tank state.
     * @returns None
     */
    void update() override;
    
    /**
     * @brief Gets the bounding dimensions of the player tank.
     * @return Point representing tank's width and height.
     */
    Point getBounds() const override;
    
    /**
     * @brief Gets the character symbol representing the player tank.
     * @return Character symbol for display.
     */
    char getSymbol() const override;
    
    /**
     * @brief Fires projectile(s) from player tank.
     * @return Pointer to created projectile, or nullptr if cannot fire.
     */
    Projectile* fire() override;
};

#endif // PLAYERTANK_H
