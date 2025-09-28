// src/model/GameObject.cpp
#ifndef GAMEOBJECT_CPP
#define GAMEOBJECT_CPP

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {return x == other.x && y == other.y;}
};

enum class Direction { UP, DOWN, LEFT, RIGHT };

class GameObject {
    protected:
        Point position;
        Direction direction;
        int speed;
        int health;
        bool destructible;

    public:
        GameObject(Point pos, Direction dir, int spd, int hp, bool destruct);
        virtual ~GameObject() = default;
        
        // Базовые методы
        virtual void move(Direction dir);
        void rotate(Direction newDirection);
        virtual void takeDamage(int damage);
        bool isDestroyed() const;
        virtual Point getBounds() const = 0; // Абстрактный метод

        Point getPosition() const { return position; }
        Direction getDirection() const { return direction; }
        
        // Виртуальные методы для переопределения
        virtual void update() = 0;
        virtual char getSymbol() const = 0; // Для отрисовки в консоли
};

#endif // GAMEOBJECT_CPP