#include "EnemyTank.h"
#include <cstdlib>
#include <algorithm>

EnemyTank::EnemyTank(Point pos, AIBehavior behav, int diff)
    : Tank(pos, Direction::DOWN, 1, 2, 1), 
      behavior(behav), difficulty(diff), 
      playerLastPosition(Point(-1, -1)),
      moveCooldown(0)
{
    setReloadTime(2);
}

void EnemyTank::setPlayerPosition(Point playerPos) { 
    playerLastPosition = playerPos; 
}

void EnemyTank::update() {
    reload();
    updateBonus();
    
    decideNextMove();
}

Point EnemyTank::getBounds() const {
    // Вражеский танк занимает 1x1 клетки
    return Point(1, 1);
}

char EnemyTank::getSymbol() const {
    // Символ вражеского танка в зависимости от направления
    switch(direction) {
        case Direction::UP: return 'A';
        case Direction::DOWN: return 'V';
        case Direction::LEFT: return '['; 
        case Direction::RIGHT: return ']';
        default: return 'E';
    }
}

bool EnemyTank::canSeePlayer() const {
    if (playerLastPosition.x == -1) return false;
    
    return (position.x == playerLastPosition.x || 
            position.y == playerLastPosition.y);
}

Direction EnemyTank::getDirectionToPlayer() const {
    if (playerLastPosition.x == -1) return direction;
    
    int dx = playerLastPosition.x - position.x;
    int dy = playerLastPosition.y - position.y;
    
    if (abs(dx) > abs(dy)) {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    } else {
        return (dy > 0) ? Direction::DOWN : Direction::UP;
    }
}

int EnemyTank::getDistanceToPlayer() const {
    if (playerLastPosition.x == -1) return -1;
    
    return abs(position.x - playerLastPosition.x) + 
           abs(position.y - playerLastPosition.y);
}

Projectile* EnemyTank::fire() {
    if (!canFire()) return nullptr;
    
    // Вражеские танки имеют разную точность в зависимости от сложности
    if (difficulty == 1 && rand() % 3 == 0) { // 33% шанс промаха
        // Стреляем в случайном направлении
        Direction randomDir = static_cast<Direction>(rand() % 4);
        rotate(randomDir);
    } else if (difficulty == 2 && rand() % 5 == 0) { // 20% шанс промаха
        // Небольшое отклонение
        Direction nearDir = getDirectionToPlayer();
        if (rand() % 2 == 0) {
            // Слегка отклоняемся
            switch (nearDir) {
                case Direction::UP: 
                case Direction::DOWN:
                    rotate(rand() % 2 == 0 ? Direction::LEFT : Direction::RIGHT);
                    break;
                case Direction::LEFT:
                case Direction::RIGHT:
                    rotate(rand() % 2 == 0 ? Direction::UP : Direction::DOWN);
                    break;
            }
        }
    }
    // Высокая сложность - всегда стреляет точно к игроку
    else if (difficulty == 3) {
        rotate(getDirectionToPlayer());
    }
    
    return Tank::fire();
}

// Приватные методы

void EnemyTank::decideNextMove() {
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

void EnemyTank::randomBehavior() {
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

void EnemyTank::aggressiveBehavior() {
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

void EnemyTank::defensiveBehavior() {
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

void EnemyTank::moveTowardsPlayer() {
    Direction toPlayer = getDirectionToPlayer();
    
    // 80% шанс движения к игроку, 20% - случайное движение
    if (rand() % 10 < 8) {
        rotate(toPlayer);
        move(toPlayer);
    } else {
        randomBehavior();
    }
}

void EnemyTank::defensiveMove() {
    Direction toPlayer = getDirectionToPlayer();
    Direction awayFromPlayer = getOppositeDirection(toPlayer);
    
    // Двигаемся от игрока
    rotate(awayFromPlayer);
    move(awayFromPlayer);
    
    // Поворачиваемся к игроку для возможного выстрела
    rotate(toPlayer);
}

void EnemyTank::cautiousApproach() {
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

void EnemyTank::findCover() {
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

void EnemyTank::attemptShot() {
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
        rotate(getDirectionToPlayer());
        fire();
    }
}

Direction EnemyTank::getOppositeDirection(Direction dir) const {
    switch (dir) {
        case Direction::UP: return Direction::DOWN;
        case Direction::DOWN: return Direction::UP;
        case Direction::LEFT: return Direction::RIGHT;
        case Direction::RIGHT: return Direction::LEFT;
        default: return dir;
    }
}

Direction EnemyTank::getPerpendicularDirection(Direction dir) const {
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

bool EnemyTank::hasClearShot() const {
    if (playerLastPosition.x == -1) return false;
    
    Point start = position;
    Point target = playerLastPosition;
    
    if (start.x == target.x) {
        return true;
    } else if (start.y == target.y) {
        return true;
    }
    
    return false;
}
