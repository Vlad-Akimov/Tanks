#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Tank.h"

class PlayerTank : public Tank {
private:
    int lives;
    int score;

public:
    PlayerTank(Point pos);
    void reset(int level);
    void addScore(int points);
    void loseLife();
    bool hasLives() const;
    
    int getLives() const;
    int getScore() const;
    
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
    Projectile* fire() override;
};

#endif // PLAYERTANK_H
