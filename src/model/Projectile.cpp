#ifndef PROJECTILE_CPP
#define PROJECTILE_CPP

#include "GameObject.cpp"
#include <vector>

class Tank; // Forward declaration

class Projectile : public GameObject {
    private:
        int damage;
        Tank* owner;
        bool processed; // Флаг, что снаряд уже обработан

    public:
        Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank)
            : GameObject(pos, dir, 0, 1, true), damage(dmg), owner(ownerTank), processed(false) {}
        
        // Геттеры
        Tank* getOwner() const { return owner; }
        int getDamage() const { return damage; }
        bool isProcessed() const { return processed; }
        void markProcessed() { processed = true; }
        
        // Реализация абстрактных методов
        void update() override {
            // Помечаем снаряд для удаления после первого обновления
            takeDamage(1);
        }
        
        Point getBounds() const override {
            return Point(1, 1);
        }
        
        char getSymbol() const override {
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
        std::vector<Point> getTrajectory(int maxRange = 20) const {
            std::vector<Point> trajectory;
            Point currentPos = position;
            
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
                
                // Проверяем выход за границы
                if (point.x < 0 || point.y < 0) break;
                
                trajectory.push_back(point);
                currentPos = point;
            }
            
            return trajectory;
        }
};

#endif // PROJECTILE_CPP