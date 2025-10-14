#ifndef TANK_CPP
#define TANK_CPP

#include "Tank.h"

Tank::Tank(Point pos, Direction dir, int spd, int hp, int fireRate)
    : GameObject(pos, dir, spd, hp, true), 
      fireRate(fireRate), reloadTime(0), currentReload(0),
      hasShield(false), doubleFire(false), speedBoost(0), bonusDuration(0) {}

// Методы танка
Projectile* Tank::fire() {
    if (!canFire()) return nullptr;
    
    // Создаем снаряд перед танком в направлении стрельбы
    Point projectilePos = position;
    
    // Смещаем позицию снаряда вперед от танка
    switch (direction) {
        case Direction::UP:
            projectilePos.y -= 1;
            break;
        case Direction::DOWN:
            projectilePos.y += 1;
            break;
        case Direction::LEFT:
            projectilePos.x -= 1;
            break;
        case Direction::RIGHT:
            projectilePos.x += 1;
            break;
    }
    
    // Сбрасываем перезарядку
    currentReload = reloadTime;
    
    // Создаем снаряд (он сам вычислит свою целевую позицию)
    return new Projectile(projectilePos, direction, 1, this);
}

void Tank::applyBonus(BonusType bonus) {
    switch (bonus) {
        case BonusType::SHIELD:
            hasShield = true;
            bonusDuration = 10; // Действует 10 ходов
            break;
            
        case BonusType::DOUBLE_FIRE:
            doubleFire = true;
            bonusDuration = 15; // Действует 15 ходов
            break;
            
        case BonusType::SPEED_BOOST:
            speedBoost = 1; // Увеличиваем скорость на 1
            bonusDuration = 20; // Действует 20 ходов
            break;
            
        case BonusType::LIFE_UP:
            // Увеличиваем здоровье
            health += 1;
            break;
    }
}

void Tank::updateBonus() {
    if (bonusDuration > 0) {
        bonusDuration--;
        if (bonusDuration == 0) {
            // Снимаем все бонусы
            hasShield = false;
            doubleFire = false;
            speedBoost = 0;
        }
    }
}

void Tank::reload() { 
    if (currentReload > 0) { 
        currentReload--;
    } 
}

bool Tank::canFire() const { 
    return currentReload == 0; 
}

// Переопределение метода получения урона с учетом щита
void Tank::takeDamage(int damage) {
    if (hasShield) { 
        return; // Щит поглощает урон
    }
    GameObject::takeDamage(damage);
}

// Переопределение метода движения с учетом ускорения
void Tank::move(Direction dir) {
    // Обновляем направление
    direction = dir;
    
    // Вычисляем фактическую скорость с учетом бонуса
    int actualSpeed = speed + speedBoost;
    
    // Ограничиваем максимальную скорость для предотвращения прохождения сквозь объекты
    if (actualSpeed > 2) actualSpeed = 2;
    
    // Двигаемся пошагово, проверяя каждую клетку
    for (int step = 0; step < actualSpeed; step++) {
        Point prevPos = position;
        
        // Вычисляем новую позицию для одного шага
        switch (dir) {
            case Direction::UP:
                position.y -= 1;
                break;
            case Direction::DOWN:
                position.y += 1;
                break;
            case Direction::LEFT:
                position.x -= 1;
                break;
            case Direction::RIGHT:
                position.x += 1;
                break;
        }
    }
}

// Геттеры
bool Tank::getHasShield() const { 
    return hasShield; 
}

bool Tank::getDoubleFire() const { 
    return doubleFire; 
}

int Tank::getBonusDuration() const { 
    return bonusDuration; 
}

// Сеттеры для настройки параметров
void Tank::setReloadTime(int reload) { 
    reloadTime = reload; 
}

void Tank::setFireRate(int rate) { 
    fireRate = rate; 
}

void Tank::setBonusDuration(int duration) { 
    bonusDuration = duration; 
}

#endif // TANK_CPP
