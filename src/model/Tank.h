#ifndef TANK_H
#define TANK_H

#include "GameObject.h"
#include "Projectile.h"
#include "Bonus.h"

class Tank : public GameObject {
protected:
    int fireRate;
    int reloadTime;
    int currentReload;
    bool hasShield;
    bool doubleFire;
    int speedBoost;
    int bonusDuration;

public:
    Tank(Point pos, Direction dir, int spd, int hp, int fireRate);
    virtual ~Tank() = default;
    
    virtual Projectile* fire();
    void applyBonus(BonusType bonus);
    void updateBonus();
    void reload();
    bool canFire() const;
    
    void takeDamage(int damage) override;
    void move(Direction dir) override;
    
    bool getHasShield() const;
    bool getDoubleFire() const;
    int getBonusDuration() const;
    
    void setReloadTime(int reload);
    void setFireRate(int rate);
    void setBonusDuration(int duration);
    
    virtual void update() override = 0;
    virtual Point getBounds() const override = 0;
    virtual char getSymbol() const override = 0;
};

#endif // TANK_H