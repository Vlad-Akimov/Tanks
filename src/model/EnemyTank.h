#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "Tank.h"
#include <stdlib.h>

enum class AIBehavior { RANDOM, AGGRESSIVE, DEFENSIVE };

enum class EnemyTankType {
    BASIC,      // Обычный танк
    FAST,       // Быстрый танк
    DAMAGE,     // Танк с двойным уроном
    ARMORED     // Танк с двойным здоровьем
};

class EnemyTank : public Tank {
private:
    AIBehavior behavior;
    int difficulty;
    EnemyTankType tankType;
    Point playerLastPosition;
    int moveCooldown;
    
    void decideNextMove();
    void randomBehavior();
    void aggressiveBehavior();
    void defensiveBehavior();
    void moveTowardsPlayer();
    void defensiveMove();
    void cautiousApproach();
    void findCover();
    void attemptShot();
    Direction getOppositeDirection(Direction dir) const;
    Direction getPerpendicularDirection(Direction dir) const;
    bool hasClearShot() const;

public:
    EnemyTank(Point pos, AIBehavior behav, int diff, EnemyTankType type = EnemyTankType::BASIC);
    void setPlayerPosition(Point playerPos);
    
    void update() override;
    void move(Direction dir) override;
    Point getBounds() const override;
    char getSymbol() const override;
    
    bool canSeePlayer() const;
    Direction getDirectionToPlayer() const;
    int getDistanceToPlayer() const;
    Projectile* fire() override;

    EnemyTankType getTankType() const;

    void applySlowEffect(int duration);
};

#endif // ENEMYTANK_H
