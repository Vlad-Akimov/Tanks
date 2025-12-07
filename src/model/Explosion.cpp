/**
 * @file Explosion.cpp
 * @author Vld251
 * @brief Implementation of timed explosion visual effects.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

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
