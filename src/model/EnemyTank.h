#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "Tank.h"

class EnemyTank : public Tank {
private:
    int difficulty;
    Point targetPosition;

public:
    EnemyTank(Point pos, int difficulty);
    
    // Методы ИИ
    void decideNextMove(const Point& playerPosition);
    void setTarget(Point target);
    
    // Реализация виртуальных методов
    void update() override;
    char getDisplayChar() const override;
    Projectile* fire() override;
};

#endif // ENEMYTANK_H