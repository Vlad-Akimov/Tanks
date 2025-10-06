#ifndef PROJECTILE_CPP
#define PROJECTILE_CPP

#include "GameObject.cpp"

class Tank; // Forward declaration

class Projectile : public GameObject {
    private:
        int damage;
        Tank* owner;

    public:
        Projectile(Point pos, Direction dir, int dmg, Tank* ownerTank)
            : GameObject(pos, dir, 0, 1, true), damage(dmg), owner(ownerTank) {}
        
        // Геттеры
        Tank* getOwner() const { return owner; }
        int getDamage() const { return damage; }
        
        // Реализация абстрактных методов
        void update() override {}
        
        Point getBounds() const override {
            // Снаряд занимает одну клетку
            return Point(1, 1);
        }
        
        char getSymbol() const override {
            // Символ снаряда в зависимости от направления
            switch(direction) {
                case Direction::UP:
                case Direction::DOWN:
                    return '|'; // Вертикальный снаряд
                case Direction::LEFT:
                case Direction::RIGHT:
                    return '-'; // Горизонтальный снаряд
                default:
                    return '*'; // По умолчанию
            }
        }
        
        // Дополнительный метод для мгновенной проверки траектории
        // Возвращает первую точку столкновения на пути снаряда
        Point calculateImpactPoint(int maxRange = 10) const {
            Point impactPos = position;
            
            // Проверяем точки по направлению выстрела до maxRange
            for (int i = 1; i <= maxRange; i++) {
                Point testPos = position;
                
                switch(direction) {
                    case Direction::UP:
                        testPos.y -= i;
                        break;
                    case Direction::DOWN:
                        testPos.y += i;
                        break;
                    case Direction::LEFT:
                        testPos.x -= i;
                        break;
                    case Direction::RIGHT:
                        testPos.x += i;
                        break;
                }
                
                // Если вышли за границы поля - возвращаем последнюю валидную позицию
                if (testPos.x < 0 || testPos.y < 0) { return impactPos; }
                
                impactPos = testPos;
            }
            
            return impactPos;
        }
};

#endif // PROJECTILE_CPP