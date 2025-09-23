// src/model/Bonus.h
#ifndef BONUS_H
#define BONUS_H

#include "GameObject.h"
#include "../utils/Constants.h"

class Tank; // Forward declaration

class Bonus : public GameObject {
private:
    Constants::BonusType type;
    int duration; // Duration in game ticks (-1 for permanent)
    bool active;
    int effectTimer;

public:
    Bonus(int x, int y, Constants::BonusType bonusType, int duration = -1);
    virtual ~Bonus() = default;
    
    virtual void update() override;
    void applyEffect(Tank* tank);
    void deactivate();
    
    // Getters
    Constants::BonusType getType() const { return type; }
    bool isActive() const { return active; }
    int getDuration() const { return duration; }
};

#endif