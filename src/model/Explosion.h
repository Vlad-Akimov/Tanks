#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"

class Explosion : public GameObject {
private:
    int lifetime;

public:
    Explosion(Point pos);
    
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
};

#endif // EXPLOSION_H
