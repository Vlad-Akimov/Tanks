#ifndef TANK_H
#define TANK_H

#include "GameObject.h"

class Projectile;

class Tank : public GameObject {
protected:
    int fireRate;
    int reloadTime;
    bool hasShield;
    bool doubleFire;

public:
    Tank(Point pos, Direction dir, int health, int speed);
    virtual ~Tank() = default;
    
    // Методы танка
    virtual Projectile* fire() = 0;
    void applyBonus(int bonusType);
    void setShield(bool active);
    void setDoubleFire(bool active);
    
    // Виртуальные методы
    virtual void update() override = 0;
    virtual char getDisplayChar() const override = 0;
};

#endif // TANK_H