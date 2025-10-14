#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

enum class ObstacleType { BRICK, STEEL, WATER, FOREST };

class Obstacle : public GameObject {
private:
    ObstacleType type;
    bool movable;

public:
    Obstacle(Point pos, ObstacleType obsType, bool movable = false);
    
    ObstacleType getType() const;
    bool isMovable() const;
    bool isDestructible() const;
    bool isPassable() const;
    bool isProjectilePassable() const;
    bool isProjectileTransparent() const;
    bool providesCover() const;
    
    void update() override;
    Point getBounds() const override;
    char getSymbol() const override;
    void takeDamage(int damage) override;
};

class SteelWall : public Obstacle {
public:
    SteelWall(Point pos);
    char getSymbol() const override;
    void takeDamage(int damage) override;
};

class Water : public Obstacle {
public:
    Water(Point pos);
    char getSymbol() const override;
};

class Forest : public Obstacle {
public:
    Forest(Point pos);
    char getSymbol() const override;
};

#endif // OBSTACLE_H
