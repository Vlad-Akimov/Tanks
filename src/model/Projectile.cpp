/**
 * @file Projectile.cpp
 * @author Vld251
 * @brief Implementation of projectile movement, collision detection, and damage calculation.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Projectile.h"
#include "Tank.h"

Projectile::Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank)
    : GameObject(pos, dir, 0, 1, true), damage(dmg), owner(ownerTank), processed(false) {}

// Геттеры
Tank* Projectile::getOwner() const { 
    return owner; 
}

int Projectile::getDamage() const { 
    return damage; 
}

bool Projectile::isProcessed() const { 
    return processed; 
}

void Projectile::markProcessed() { 
    processed = true; 
}

// Реализация абстрактных методов
void Projectile::update() {
    // Помечаем снаряд для удаления после первого обновления
    takeDamage(1);
}

Point Projectile::getBounds() const {
    return Point(1, 1);
}

char Projectile::getSymbol() const {
    switch(direction) {
        case Direction::UP:
        case Direction::DOWN:
            return '|';
        case Direction::LEFT:
        case Direction::RIGHT:
            return '-';
        default:
            return '*';
    }
}

// Вычисление всех точек траектории
std::vector<Point> Projectile::getTrajectory(int maxRange) const {
    std::vector<Point> trajectory;
    
    for (int i = 0; i <= maxRange; i++) {
        Point point = position;
        
        switch(direction) {
            case Direction::UP:
                point.y -= i;
                break;
            case Direction::DOWN:
                point.y += i;
                break;
            case Direction::LEFT:
                point.x -= i;
                break;
            case Direction::RIGHT:
                point.x += i;
                break;
        }
        
        trajectory.push_back(point);
    }
    
    return trajectory;
}

void Projectile::setDamage(int newDamage) {
    damage = newDamage;
}
