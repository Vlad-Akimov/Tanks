#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Point.h"

enum class Direction { UP, DOWN, LEFT, RIGHT };

class GameObject {
protected:
    Point position;
    Direction direction;
    int speed;
    int health;
    bool destructible;
    bool destroyed;

public:
    GameObject(Point pos, Direction dir, int health, bool destructible);
    virtual ~GameObject() = default;
    
    // Базовые методы
    virtual void move(Direction newDirection);
    void rotate(Direction newDirection);
    virtual void takeDamage(int damage);
    bool isDestroyed() const;
    Point getPosition() const;
    Direction getDirection() const;
    
    // Виртуальные методы для переопределения
    virtual void update() = 0;
    virtual char getDisplayChar() const = 0;
};

#endif // GAMEOBJECT_H