#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"

class Tank; // Forward declaration

class Projectile : public GameObject {
private:
    int damage;
    Tank* owner;

public:
    Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank);
    
    // Геттеры
    Tank* getOwner() const { return owner; }
    int getDamage() const { return damage; }
    
    // Реализация абстрактных методов
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
};

#endif // PROJECTILE_H