/**
 * @file GameObject.cpp
 * @author Vld251
 * @brief Implementation of base game object methods for movement, rotation, and damage.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GameObject.h"

Point::Point(int x, int y) : x(x), y(y) {}

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return x != other.x || y != other.y;
}

GameObject::GameObject(Point pos, Direction dir, int spd, int hp, bool destruct) 
    : position(pos), direction(dir), speed(spd), health(hp), destructible(destruct) {}

void GameObject::move(Direction dir) {
    // Обновляем направление
    direction = dir;
    
    // Вычисляем новую позицию в зависимости от направления
    switch (dir) {
        case Direction::UP:
            position.y -= speed;
            break;
        case Direction::DOWN:
            position.y += speed;
            break;
        case Direction::LEFT:
            position.x -= speed;
            break;
        case Direction::RIGHT:
            position.x += speed;
            break;
    }
}

void GameObject::rotate(Direction newDirection) {
    direction = newDirection;
}

void GameObject::takeDamage(int damage) {
    if (destructible) {
        health -= damage;
        if (health < 0) health = 0;
    }
}

bool GameObject::isDestroyed() const {
    return destructible && health <= 0;
}

void GameObject::setDestroyed(bool destroyed) {
    if (destructible) {
        health = destroyed ? 0 : 3; // Восстанавливаем здоровье
    }
}

Point GameObject::getPosition() const { 
    return position; 
}

Direction GameObject::getDirection() const { 
    return direction; 
}

int GameObject::getSpeed() const { 
    return speed; 
}

int GameObject::getHealth() const { 
    return health; 
}

bool GameObject::isDestructible() const { 
    return destructible; 
}

void GameObject::setPosition(Point newPos) { 
    position = newPos; 
}

void GameObject::setHealth(int newHealth) { 
    health = newHealth; 
}
