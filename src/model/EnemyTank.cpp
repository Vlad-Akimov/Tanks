#ifndef ENEMYTANK_CPP
#define ENEMYTANK_CPP

#include "Tank.cpp"

enum class AIBehavior { RANDOM, AGGRESSIVE, DEFENSIVE };

class EnemyTank : public Tank {
private:
    AIBehavior behavior;
    int difficulty;
    Point playerLastPosition;
    
    void decideNextMove();

public:
    EnemyTank(Point pos, AIBehavior behav, int diff);
    
    // Методы ИИ
    void setPlayerPosition(Point playerPos);
    
    // Реализация абстрактных методов
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
};

#endif // ENEMYTANK_CPP