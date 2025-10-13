#ifndef BONUS_H
#define BONUS_H

#include "GameObject.h"

class PlayerTank;

enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };

class Bonus : public GameObject {
private:
    BonusType type;
    int duration;
    bool active;
    int activationTime;

public:
    Bonus(Point pos, BonusType bonusType, int dur = 0);
    void applyEffect(PlayerTank* playerTank);
    void deactivate();
    bool isActive() const;
    bool isProjectilePassable() const;
    BonusType getType() const;
    
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
};

#endif // BONUS_H