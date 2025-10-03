#ifndef ENEMYTANK_CPP
#define ENEMYTANK_CPP

#include "Tank.cpp"
#include <stdlib.h>

enum class AIBehavior { RANDOM, AGGRESSIVE, DEFENSIVE };

class EnemyTank : public Tank {
    private:
        AIBehavior behavior;
        int difficulty;
        Point playerLastPosition;
        
        void decideNextMove();

    public:
        EnemyTank(Point pos, AIBehavior behav, int diff)
            : Tank(pos, Direction::DOWN, 1, 2, 1), 
              behavior(behav), difficulty(diff), 
              playerLastPosition(Point(-1, -1)) 
        {
            setReloadTime(2); // Более медленная перезарядка чем у игрока
        }
        
        // Методы ИИ
        void setPlayerPosition(Point playerPos) {
            playerLastPosition = playerPos;
        }
        
        // Реализация абстрактных методов
        void update() override {
            // Обновляем перезарядку и бонусы каждый ход
            reload();
            updateBonus();
            
            // В пошаговой игре ИИ принимает решение о действии
            // Основная логика будет вызываться из GameWorld
        }
        
        Point getBounds() const override {
            // Вражеский танк занимает 2x2 клетки
            return Point(2, 2);
        }
        
        char getSymbol() const override {
            // Символ вражеского танка в зависимости от направления
            switch(direction) {
                case Direction::UP: return 'A'; // A как Alien/Aggressor
                case Direction::DOWN: return 'V'; // V как враг (снизу)
                case Direction::LEFT: return '['; // [ как левый враг
                case Direction::RIGHT: return ']'; // ] как правый враг
                default: return 'E'; // E как Enemy
            }
        }
        
        // Дополнительные методы для ИИ
        
        // Проверка видимости игрока
        bool canSeePlayer() const {
            if (playerLastPosition.x == -1) return false;
            
            // Простая проверка прямой видимости
            // В реальной реализации нужно учитывать препятствия
            return (position.x == playerLastPosition.x || 
                    position.y == playerLastPosition.y);
        }
        
        // Получение направления к игроку
        Direction getDirectionToPlayer() const {
            if (playerLastPosition.x == -1) return direction;
            
            int dx = playerLastPosition.x - position.x;
            int dy = playerLastPosition.y - position.y;
            
            // Определяем преобладающее направление
            if (abs(dx) > abs(dy)) {
                return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
            } else {
                return (dy > 0) ? Direction::DOWN : Direction::UP;
            }
        }
        
        // Метод для получения расстояния до игрока
        int getDistanceToPlayer() const {
            if (playerLastPosition.x == -1) return -1;
            
            return abs(position.x - playerLastPosition.x) + 
                   abs(position.y - playerLastPosition.y);
        }
        
        // Переопределение метода fire для вражеских танков
        Projectile* fire() override {
            if (!canFire()) return nullptr;
            
            // Вражеские танки имеют разную точность в зависимости от сложности
            // Случайный шанс промаха для низкой сложности
            if (difficulty == 1 && rand() % 3 == 0) { // 33% шанс промаха
                return nullptr;
            }
            
            // Для средней сложности - небольшой шанс промаха
            if (difficulty == 2 && rand() % 5 == 0) { // 20% шанс промаха
                return nullptr;
            }
            
            // Высокая сложность - всегда стреляет точно
            return Tank::fire();
        }
        
        // Метод для получения типа поведения
        AIBehavior getBehavior() const { return behavior; }
        
        // Метод для получения уровня сложности
        int getDifficulty() const { return difficulty; }
};

#endif // ENEMYTANK_CPP