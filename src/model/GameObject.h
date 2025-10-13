#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

struct Point {
    int x, y;
    Point(int x = 0, int y = 0);
    bool operator==(const Point& other) const;
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
    
    virtual void move(Direction dir);
    void rotate(Direction newDirection);
    virtual void takeDamage(int damage);
    bool isDestroyed() const;
    void setDestroyed(bool destroyed);
    
    virtual Point getBounds() const = 0;
    virtual void update() = 0;
    virtual char getSymbol() const = 0;
    
    Point getPosition() const;
    Direction getDirection() const;
    int getSpeed() const;
    int getHealth() const;
    bool isDestructible() const;
    
    void setPosition(Point newPos);
    void setHealth(int newHealth);
};

#endif // GAMEOBJECT_H