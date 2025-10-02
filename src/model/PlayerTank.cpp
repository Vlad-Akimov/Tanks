#ifndef PLAYERTANK_CPP
#define PLAYERTANK_CPP

#include "Tank.cpp"

class PlayerTank : public Tank {
    private:
        int lives;
        int score;

    public:
        PlayerTank(Point pos) 
            : Tank(pos, Direction::UP, 1, 3, 2), lives(3), score(0) {
            setReloadTime(1); // Перезарядка 1 ход
        }
        
        // Методы игрока
        void addScore(int points) {
            score += points;
        }
        
        void loseLife() {
            if (lives > 0) {
                lives--;
                // Сбрасываем здоровье при потере жизни
                health = 3;
                // Сбрасываем бонусы
                hasShield = false;
                doubleFire = false;
                speedBoost = 0;
                bonusDuration = 0;
            }
        }
        
        bool hasLives() const {
            return lives > 0;
        }
        
        // Геттеры
        int getLives() const { return lives; }
        int getScore() const { return score; }
        
        // Реализация абстрактных методов
        void update() override {
            // Обновляем перезарядку и бонусы каждый ход
            reload();
            updateBonus();
        }
        
        Point getBounds() const override {
            // Танк игрока занимает 2x2 клетки
            return Point(2, 2);
        }
        
        char getSymbol() const override {
            // Символ танка игрока в зависимости от направления
            switch(direction) {
                case Direction::UP: return '^';
                case Direction::DOWN: return 'v';
                case Direction::LEFT: return '<';
                case Direction::RIGHT: return '>';
                default: return 'T';
            }
        }
        
        // Переопределение метода fire для двойного огня
        Projectile* fire() override {
            Projectile* firstShot = Tank::fire();
            
            if (firstShot && doubleFire) {
                // Создаем второй снаряд для двойного огня
                Point secondPos = firstShot->getPosition();
                
                // Смещаем второй снаряд в сторону
                switch (direction) {
                    case Direction::UP:
                    case Direction::DOWN:
                        secondPos.x += 1; // Справа от первого
                        break;
                    case Direction::LEFT:
                    case Direction::RIGHT:
                        secondPos.y += 1; // Снизу от первого
                        break;
                }
                
                // Второй снаряд создается, но не влияет на перезарядку
                return new Projectile(secondPos, direction, 1, this);
            }
            
            return firstShot;
        }
};

#endif // PLAYERTANK_CPP