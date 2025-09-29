#ifndef OBSTACLE_CPP
#define OBSTACLE_CPP

#include "GameObject.cpp"

enum class ObstacleType { BRICK, STEEL, WATER, FOREST };

class Obstacle : public GameObject {
    private:
        ObstacleType type;
        bool movable;

    public:
        Obstacle(Point pos, ObstacleType obsType, bool movable = false)
            : GameObject(pos, Direction::UP, 0, 1, false), 
              type(obsType), movable(movable) 
        {
            // Устанавливаем параметры в зависимости от типа препятствия
            switch(type) {
                case ObstacleType::BRICK:
                    destructible = true;
                    health = 1; // Кирпичная стена разрушается с одного попадания
                    break;
                case ObstacleType::STEEL:
                    destructible = true;
                    health = 3; // Стальная стена требует несколько попаданий
                    break;
                case ObstacleType::WATER:
                case ObstacleType::FOREST:
                    destructible = false;
                    health = 1;
                    break;
            }
        }
        
        // Геттеры
        ObstacleType getType() const { return type; }
        bool isMovable() const { return movable; }
        
        bool isDestructible() const {
            return destructible;
        }
        
        bool isPassable() const {
            // Вода непроходима для танков, но проходима для снарядов
            // Лес проходим для всех, но обеспечивает укрытие
            switch(type) {
                case ObstacleType::WATER:
                    return false; // Непроходимо для танков
                case ObstacleType::FOREST:
                    return true;  // Проходимо, но скрывает танки
                case ObstacleType::BRICK:
                case ObstacleType::STEEL:
                    return false; // Непроходимо
                default:
                    return true;
            }
        }
        
        // Реализация абстрактных методов
        void update() override {
            // В пошаговой игре препятствия не требуют постоянного обновления
            // Основная логика будет обрабатываться в GameWorld::checkCollisions()
        }
        
        Point getBounds() const override {
            // Все препятствия занимают одну клетку
            return Point(1, 1);
        }
        
        char getSymbol() const override {
            switch(type) {
                case ObstacleType::BRICK:
                    return '#';  // Кирпичная стена
                case ObstacleType::STEEL:
                    return 'X';  // Стальная стена
                case ObstacleType::WATER:
                    return '~';  // Вода
                case ObstacleType::FOREST:
                    return '*';  // Лес
                default:
                    return '?';
            }
        }
        
        // Переопределение метода получения урона для препятствий
        void takeDamage(int damage) override {
            if (destructible) {
                health -= damage;
                if (health < 0) health = 0;
                
                // Дополнительные эффекты при разрушении
                if (isDestroyed()) {
                    // Можно добавить эффекты разрушения или анимацию
                }
            }
        }
        
        // Метод для проверки, может ли снаряд пройти через препятствие
        bool isProjectilePassable() const {
            switch(type) {
                case ObstacleType::WATER:
                    return true;  // Снаряды проходят через воду
                case ObstacleType::FOREST:
                    return true;  // Снаряды проходят через лес
                default:
                    return false; // Снаряды не проходят через стены
            }
        }
        
        // Метод для проверки, скрывает ли препятствие объекты за собой
        bool providesCover() const {
            return type == ObstacleType::FOREST; // Лес скрывает танки
        }
};

class SteelWall : public Obstacle {
    public:
        SteelWall(Point pos) : Obstacle(pos, ObstacleType::STEEL, false) {}
        
        char getSymbol() const override { 
            if (isDestroyed()) return ' ';
            return 'X'; 
        }
        
        void takeDamage(int damage) override {
            // Стальная стена более устойчива к повреждениям
            if (isDestructible()) {
                health -= (damage > 1) ? 1 : damage; // Максимум 1 урон за выстрел
                if (health < 0) health = 0;
            }
        }
};

class Water : public Obstacle {
    public:
        Water(Point pos) : Obstacle(pos, ObstacleType::WATER, false) {}
        
        char getSymbol() const override { return '~'; }
};

class Forest : public Obstacle {
    public:
        Forest(Point pos) : Obstacle(pos, ObstacleType::FOREST, false) {}
        
        char getSymbol() const override { return '*'; }
};

#endif // OBSTACLE_CPP