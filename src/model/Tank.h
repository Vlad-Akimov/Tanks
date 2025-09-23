#ifndef TANK_H
#define TANK_H

#include "GameObject.h"

class Projectile;

class Tank : public GameObject {
protected:
    int speed;
    int fireRate;
    int reloadTimer;
    bool hasShield;
    bool doubleFire;

public:
    Tank(int x, int y, Constants::Direction dir, char displayChar);
    virtual void update() override;
    virtual Projectile* fire();
    void applyBonus(Constants::BonusType bonus);
    void move(Constants::Direction newDirection);
};

#endif