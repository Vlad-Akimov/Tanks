// src/model/GameObject.h
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "../utils/Rectangle.h"
#include "../utils/Constants.h"

class GameObject {
protected:
    int x, y;
    Constants::Direction direction;
    int health;
    bool destructible;
    char displayChar;  // Символ для отображения в консоли

public:
    GameObject(int x, int y, Constants::Direction dir, char displayChar);
    virtual ~GameObject() = default;
    
    virtual void update() = 0;
    virtual void takeDamage(int damage);
    bool isDestroyed() const;
    Rectangle getBounds() const;
    
    // Геттеры
    int getX() const { return x; }
    int getY() const { return y; }
    char getDisplayChar() const { return displayChar; }
};

#endif