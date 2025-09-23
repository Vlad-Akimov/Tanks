// src/model/Projectile.h
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"

class Tank; // Forward declaration

class Projectile : public GameObject {
private:
    int damage;
    Tank* owner;
    int maxDistance;
    int distanceTraveled;

public:
    Projectile(int x, int y, Constants::Direction dir, int dmg, Tank* owningTank, int maxDist = 100);
    virtual ~Projectile() = default;
    
    virtual void update() override;
    virtual void takeDamage(int damage) override;
    
    // Getters
    int getDamage() const { return damage; }
    Tank* getOwner() const { return owner; }
    int getDistanceTraveled() const { return distanceTraveled; }
    int getMaxDistance() const { return maxDistance; }
};

#endif