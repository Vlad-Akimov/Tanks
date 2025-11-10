#include "Explosion.h"

Explosion::Explosion(Point pos) 
    : GameObject(pos, Direction::UP, 0, 1, true), lifetime(1) {}

void Explosion::update() {
    lifetime--;
    if (lifetime <= 0) {
        setDestroyed(true);
    }
}

Point Explosion::getBounds() const {
    return Point(1, 1);
}

char Explosion::getSymbol() const {
    return 'O';
}
