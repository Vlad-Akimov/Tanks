// src/model/PlayerTank.h
#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Tank.h"
#include "../utils/Constants.h"

class PlayerTank : public Tank {
private:
    int lives;
    int score;
    int respawnTimer;
    bool isRespawning;

public:
    PlayerTank(int x, int y, Constants::Direction dir = Constants::Direction::UP);
    virtual ~PlayerTank() = default;
    
    virtual void update() override;
    void respawn(int x, int y);
    void addScore(int points);
    void loseLife();
    
    // Getters
    int getLives() const { return lives; }
    int getScore() const { return score; }
    bool getIsRespawning() const { return isRespawning; }
    
    // Setters
    void setLives(int newLives) { lives = newLives; }
    void setScore(int newScore) { score = newScore; }
};

#endif