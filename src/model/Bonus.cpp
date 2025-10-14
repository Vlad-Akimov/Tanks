#ifndef BONUS_CPP
#define BONUS_CPP

#include "Bonus.h"
#include "PlayerTank.h"

// Реализация Bonus

Bonus::Bonus(Point pos, BonusType bonusType, int dur)
    : GameObject(pos, Direction::UP, 0, 1, true), 
      type(bonusType), duration(dur), active(true), activationTime(0) {}

void Bonus::applyEffect(PlayerTank* playerTank) {
    if (!playerTank || !active) return;

    switch(type) {
        case BonusType::SHIELD:
            playerTank->applyBonus(BonusType::SHIELD);
            break;
            
        case BonusType::DOUBLE_FIRE:
            playerTank->applyBonus(BonusType::DOUBLE_FIRE);
            break;
            
        case BonusType::SPEED_BOOST:
            playerTank->applyBonus(BonusType::SPEED_BOOST);
            break;
            
        case BonusType::LIFE_UP:
            // Добавляем жизнь игроку
            playerTank->applyBonus(BonusType::LIFE_UP);
            break;
    }
    // Деактивируем бонус после применения
    deactivate();
}

void Bonus::deactivate() { 
    active = false; 
}

bool Bonus::isActive() const { 
    return active; 
}

bool Bonus::isProjectilePassable() const { 
    return true; 
}

BonusType Bonus::getType() const { 
    return type; 
}

void Bonus::update() {
    // Бонусы не обновляются самостоятельно
}

Point Bonus::getBounds() const {
    // Бонус занимает одну клетку
    return Point(1, 1);
}

char Bonus::getSymbol() const {
    switch(type) {
        case BonusType::SHIELD: return 'S';
        case BonusType::DOUBLE_FIRE: return 'F';
        case BonusType::SPEED_BOOST: return 'B';
        case BonusType::LIFE_UP: return 'L';
        default: return '?';
    }
}

#endif // BONUS_CPP
