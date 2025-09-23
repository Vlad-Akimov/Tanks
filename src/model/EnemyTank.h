// src/model/EnemyTank.h
#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "Tank.h"
#include "../utils/Constants.h"

class EnemyTank : public Tank {
private:
    Constants::AIBehavior behavior;
    int difficulty;
    int moveCooldown;
    int decisionTimer;

public:
    EnemyTank(int x, int y, Constants::Direction dir, 
              Constants::AIBehavior aiBehavior = Constants::AIBehavior::PATROL, 
              int diff = 1);
    virtual ~EnemyTank() = default;
    
    virtual void update() override;
    void decideNextMove();
    
    // Getters
    Constants::AIBehavior getBehavior() const { return behavior; }
    int getDifficulty() const { return difficulty; }
};

#endif