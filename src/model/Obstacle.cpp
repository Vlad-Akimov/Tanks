// src/model/Obstacle.cpp
#ifndef OBSTACLE_CPP
#define OBSTACLE_CPP

#include "GameObject.cpp"

enum class ObstacleType { BRICK, STEEL, WATER, FOREST };

class Obstacle : public GameObject {
    private:
        ObstacleType type;
        bool movable;

    public:
        Obstacle(Point pos, ObstacleType obsType, bool movable = false);
        
        // Геттеры
        ObstacleType getType() const { return type; }
        bool isMovable() const { return movable; }
        bool isDestructible() const;
        bool isPassable() const;
        
        // Реализация абстрактных методов
        void update() override;
        Point getBounds() const override;
        char getSymbol() const override;
};

#endif // OBSTACLE_CPP