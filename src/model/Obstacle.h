#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

enum class ObstacleType { BRICK, STEEL, WATER, FOREST };

class Obstacle : public GameObject {
private:
    ObstacleType type;
    bool movable;

public:
    Obstacle(Point pos, ObstacleType type, bool movable = false);
    
    // Методы препятствий
    ObstacleType getType() const;
    bool isMovable() const;
    bool isPassable() const;
    
    // Реализация виртуальных методов
    void update() override;
    void takeDamage(int damage) override;
    char getDisplayChar() const override;
};

#endif // OBSTACLE_H