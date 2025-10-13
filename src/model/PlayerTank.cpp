#ifndef PLAYERTANK_CPP
#define PLAYERTANK_CPP

#include "PlayerTank.h"

PlayerTank::PlayerTank(Point pos): Tank(pos, Direction::UP, 1, 3, 2), lives(3), score(0) {
    setReloadTime(1);
}

void PlayerTank::reset() {
    // Сбрасываем позицию и направление
    setPosition(Point(40 / 2, 20 - 3));
    rotate(Direction::UP);
    
    // Сбрасываем здоровье и жизни
    setHealth(3);
    
    // Сбрасываем счет
    score = 0;
    
    // Сбрасываем бонусы
    hasShield = false;
    doubleFire = false;
    speedBoost = 0;
    bonusDuration = 0;
    currentReload = 0;
    
    // Сбрасываем флаг уничтожения
    setDestroyed(false);
}

// Методы игрока
void PlayerTank::addScore(int points) { 
    score += points; 
}

void PlayerTank::loseLife() {
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

bool PlayerTank::hasLives() const { 
    return lives > 0; 
}

// Геттеры
int PlayerTank::getLives() const { 
    return lives; 
}

int PlayerTank::getScore() const { 
    return score; 
}

// Реализация абстрактных методов
void PlayerTank::update() {
    // Обновляем перезарядку и бонусы каждый ход
    reload();
    updateBonus();
}

Point PlayerTank::getBounds() const {
    // Танк игрока занимает 1x1 клетки
    return Point(1, 1);
}

char PlayerTank::getSymbol() const {
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
Projectile* PlayerTank::fire() {
    Projectile* firstShot = Tank::fire();
    
    if (firstShot && doubleFire) {
        // Создаем второй снаряд для двойного огня
        Point secondPos = firstShot->getPosition();
        
        // Второй снаряд создается, но не влияет на перезарядку
        return new Projectile(secondPos, direction, 2, this);
    }
    
    return firstShot;
}

#endif // PLAYERTANK_CPP