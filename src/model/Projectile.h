#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <vector>

class Tank;

class Projectile : public GameObject {
private:
    int damage;
    Tank* owner;
    bool processed;

public:
    Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank);
    
    Tank* getOwner() const;
    int getDamage() const;
    bool isProcessed() const;
    void markProcessed();
    
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
    std::vector<Point> getTrajectory(int maxRange = 20) const;
};

#endif // PROJECTILE_H
