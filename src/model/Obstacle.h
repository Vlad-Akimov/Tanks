// src/model/Obstacle.h
#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"
#include "../utils/Constants.h"

class Obstacle : public GameObject {
private:
    Constants::ObstacleType type;
    bool movable;
    bool permeable; // Can projectiles pass through?

public:
    Obstacle(int x, int y, Constants::ObstacleType obstacleType);
    virtual ~Obstacle() = default;
    
    virtual void update() override;
    virtual void takeDamage(int damage) override;
    
    // Getters
    Constants::ObstacleType getType() const { return type; }
    bool isMovable() const { return movable; }
    bool isPermeable() const { return permeable; }
    
    // Specific obstacle behaviors
    bool canBeDestroyed() const;
    bool canTankPass() const;
    bool canProjectilePass() const;
};

#endif