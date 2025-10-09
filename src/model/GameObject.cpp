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
        GameObject(Point pos, Direction dir, int spd, int hp, bool destruct) 
            : position(pos), direction(dir), speed(spd), health(hp), destructible(destruct) {}
        
        virtual ~GameObject() = default;
        
        // Базовые методы
        virtual void move(Direction dir) {
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
        
        void rotate(Direction newDirection) {
            direction = newDirection;
        }
        
        virtual void takeDamage(int damage) {
            if (destructible) {
                health -= damage;
                if (health < 0) health = 0;
            }
        }
        
        bool isDestroyed() const {
            return destructible && health <= 0;
        }

        void setDestroyed(bool destroyed) {
            if (destructible) {
                health = destroyed ? 0 : 3; // Восстанавливаем здоровье
            }
        }
        
        virtual Point getBounds() const = 0; // Абстрактный метод
        
        // Виртуальные методы для переопределения
        virtual void update() = 0;
        virtual char getSymbol() const = 0; // Для отрисовки в консоли
        
        // Геттеры для доступа к protected полям
        Point getPosition() const { return position; }
        Direction getDirection() const { return direction; }
        int getSpeed() const { return speed; }
        int getHealth() const { return health; }
        bool isDestructible() const { return destructible; }
        
        // Сеттеры
        void setPosition(Point newPos) { position = newPos; }
        void setHealth(int newHealth) { health = newHealth; }
};

#endif // GAMEOBJECT_CPP