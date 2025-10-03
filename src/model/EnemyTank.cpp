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
        int moveCooldown; // Задержка между движениями
        int lastMoveDirection; // Для отслеживания последнего направления
        
        void decideNextMove() {
            if (moveCooldown > 0) {
                moveCooldown--;
                return;
            }

            // Сбрасываем cooldown
            moveCooldown = 1;

            // Если игрок не обнаружен, используем случайное поведение
            if (playerLastPosition.x == -1 || !canSeePlayer()) {
                randomBehavior();
                return;
            }

            // Поведение в зависимости от типа ИИ
            switch (behavior) {
                case AIBehavior::RANDOM:
                    randomBehavior();
                    break;
                case AIBehavior::AGGRESSIVE:
                    aggressiveBehavior();
                    break;
                case AIBehavior::DEFENSIVE:
                    defensiveBehavior();
                    break;
            }

            // Случайный шанс выстрела в зависимости от сложности
            attemptShot();
        }

    // Вспомогательные методы для различных типов поведения
        void randomBehavior() {
            // 70% шанс движения, 30% шанс смены направления
            if (rand() % 10 < 7) {
                // Двигаемся в текущем направлении
                move(direction);
            } else {
                // Случайно меняем направление
                Direction newDir = static_cast<Direction>(rand() % 4);
                rotate(newDir);
            }
        }

        void aggressiveBehavior() {
            Direction toPlayer = getDirectionToPlayer();
            int distance = getDistanceToPlayer();
            
            if (distance <= 3) {
                // Близкая дистанция - отступаем
                defensiveMove();
            } else if (distance <= 6) {
                // Средняя дистанция - стремимся к оптимальной позиции
                if (canFire() && hasClearShot()) {
                    // Уже в хорошей позиции - остаемся на месте
                    rotate(toPlayer);
                } else {
                    // Двигаемся к игроку
                    moveTowardsPlayer();
                }
            } else {
                // Дальняя дистанция - агрессивно приближаемся
                moveTowardsPlayer();
            }
        }

        void defensiveBehavior() {
            Direction toPlayer = getDirectionToPlayer();
            int distance = getDistanceToPlayer();
            
            if (distance <= 2) {
                // Слишком близко - отступаем
                defensiveMove();
            } else if (distance <= 5) {
                // Идеальная дистанция для обороны
                if (hasClearShot()) {
                    rotate(toPlayer);
                } else {
                    // Ищем укрытие или лучшую позицию
                    findCover();
                }
            } else {
                // Дальняя дистанция - осторожно приближаемся
                cautiousApproach();
            }
        }

        void moveTowardsPlayer() {
            Direction toPlayer = getDirectionToPlayer();
            
            // 80% шанс движения к игроку, 20% - случайное движение
            if (rand() % 10 < 8) {
                rotate(toPlayer);
                move(toPlayer);
            } else {
                randomBehavior();
            }
        }

        void defensiveMove() {
            Direction toPlayer = getDirectionToPlayer();
            Direction awayFromPlayer = getOppositeDirection(toPlayer);
            
            // Двигаемся от игрока
            rotate(awayFromPlayer);
            move(awayFromPlayer);
            
            // Поворачиваемся к игроку для возможного выстрела
            rotate(toPlayer);
        }

        void cautiousApproach() {
            Direction toPlayer = getDirectionToPlayer();
            int distance = getDistanceToPlayer();
            
            if (distance > 8) {
                // Быстрое приближение на больших дистанциях
                moveTowardsPlayer();
            } else {
                // Медленное осторожное приближение
                if (rand() % 3 == 0) { // 33% шанс движения
                    rotate(toPlayer);
                    move(toPlayer);
                } else {
                    // Остаемся на месте, но следим за игроком
                    rotate(toPlayer);
                }
            }
        }

        void findCover() {
            // Простой алгоритм поиска укрытия
            // В реальной реализации нужно анализировать карту
            
            Direction toPlayer = getDirectionToPlayer();
            
            // 50% шанс движения перпендикулярно направлению к игроку
            if (rand() % 2 == 0) {
                Direction perpendicular = getPerpendicularDirection(toPlayer);
                rotate(perpendicular);
                move(perpendicular);
            } else {
                // Двигаемся в случайном направлении
                randomBehavior();
            }
        }

        void attemptShot() {
            if (!canFire()) return;
            
            int shotChance = 0;
            
            // Вероятность выстрела в зависимости от поведения и сложности
            switch (behavior) {
                case AIBehavior::AGGRESSIVE:
                    shotChance = 70 + (difficulty * 10); // 70-90%
                    break;
                case AIBehavior::DEFENSIVE:
                    shotChance = 40 + (difficulty * 10); // 40-60%
                    break;
                case AIBehavior::RANDOM:
                    shotChance = 30 + (difficulty * 10); // 30-50%
                    break;
            }
            
            if (hasClearShot() && (rand() % 100 < shotChance)) {
                // Выстрел произойдет автоматически в GameWorld
                // Здесь просто отмечаем намерение
                rotate(getDirectionToPlayer());
            }
        }

        // Вспомогательные методы для работы с направлениями
        Direction getOppositeDirection(Direction dir) const {
            switch (dir) {
                case Direction::UP: return Direction::DOWN;
                case Direction::DOWN: return Direction::UP;
                case Direction::LEFT: return Direction::RIGHT;
                case Direction::RIGHT: return Direction::LEFT;
                default: return dir;
            }
        }

        Direction getPerpendicularDirection(Direction dir) const {
            // Возвращает случайное перпендикулярное направление
            switch (dir) {
                case Direction::UP:
                case Direction::DOWN:
                    return (rand() % 2 == 0) ? Direction::LEFT : Direction::RIGHT;
                case Direction::LEFT:
                case Direction::RIGHT:
                    return (rand() % 2 == 0) ? Direction::UP : Direction::DOWN;
                default: return dir;
            }
        }

        bool hasClearShot() const {
            if (playerLastPosition.x == -1) return false;
            
            // Простая проверка прямой видимости
            // В реальной реализации нужно проверять препятствия на линии огня
            return (position.x == playerLastPosition.x || 
                    position.y == playerLastPosition.y);
        }

    public:
        EnemyTank(Point pos, AIBehavior behav, int diff)
            : Tank(pos, Direction::DOWN, 1, 2, 1), 
            behavior(behav), difficulty(diff), 
            playerLastPosition(Point(-1, -1)),
            moveCooldown(0), lastMoveDirection(-1)
        {
            setReloadTime(2);
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
            
            // Принимаем решение о следующем действии
            decideNextMove();
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