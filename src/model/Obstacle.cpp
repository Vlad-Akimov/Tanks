/**
 * @file Obstacle.cpp
 * @author Vld251
 * @brief Implementation of environmental obstacles with different passability and durability.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Obstacle.h"

Obstacle::Obstacle(Point pos, ObstacleType obsType, bool movable)
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
ObstacleType Obstacle::getType() const { return type; }
bool Obstacle::isMovable() const { return movable; }
bool Obstacle::isDestructible() const { return destructible; }

bool Obstacle::isPassable() const {
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
void Obstacle::update() {}

Point Obstacle::getBounds() const {
    // Все препятствия занимают одну клетку
    return Point(1, 1);
}

char Obstacle::getSymbol() const {
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
void Obstacle::takeDamage(int damage) {
    if (destructible) {
        health -= damage;
        if (health < 0) health = 0;
        
        if (isDestroyed()) {
            // Можно добавить эффекты разрушения или анимацию
        }
    }
}

// Метод для проверки, может ли снаряд пройти через препятствие
bool Obstacle::isProjectilePassable() const {
    switch(type) {
        case ObstacleType::WATER:
        case ObstacleType::FOREST:
            return true;  // Снаряды проходят через воду и лес
        case ObstacleType::BRICK:
        case ObstacleType::STEEL:
            return false; // Снаряды не проходят через стены
        default:
            return false;
    }
}

// Метод для проверки, является ли препятствие полностью игнорируемым для снарядов
bool Obstacle::isProjectileTransparent() const {
    return type == ObstacleType::WATER || type == ObstacleType::FOREST;
}

// Метод для проверки, скрывает ли препятствие объекты за собой
bool Obstacle::providesCover() const {
    return type == ObstacleType::FOREST; // Лес скрывает танки
}

// Реализации производных классов
SteelWall::SteelWall(Point pos) : Obstacle(pos, ObstacleType::STEEL, false) {}

char SteelWall::getSymbol() const { 
    if (isDestroyed()) return ' ';
    return 'X'; 
}

void SteelWall::takeDamage(int damage) {
    if (isDestructible()) {
        health -= (damage > 2) ? 2 : damage;
        if (health < 0) health = 0;
    }
}

Water::Water(Point pos) : Obstacle(pos, ObstacleType::WATER, false) {}

char Water::getSymbol() const { return '~'; }

Forest::Forest(Point pos) : Obstacle(pos, ObstacleType::FOREST, false) {}

char Forest::getSymbol() const { return '*'; }
