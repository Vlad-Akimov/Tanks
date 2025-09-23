#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Tank.h"

class PlayerTank : public Tank {
private:
    int lives;
    int score;

public:
    PlayerTank(Point pos);
    
    // Методы игрока
    void addScore(int points);
    void loseLife();
    bool hasLives() const;
    int getLives() const;
    int getScore() const;
    
    // Реализация виртуальных методов
    void update() override;
    char getDisplayChar() const override;
    Projectile* fire() override;
};

#endif // PLAYERTANK_H