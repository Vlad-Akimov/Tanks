// src/model/Tank.cpp
#ifndef TANK_CPP
#define TANK_CPP

#include "GameObject.cpp"
#include "Projectile.cpp"
#include "Bonus.cpp"

class GameWorld;
class Obstacle;

class Tank : public GameObject {
    protected:
        int fireRate;
        int reloadTime;
        int currentReload;
        bool hasShield;
        bool doubleFire;
        bool speedBoost;
        int speedBoostDuration;

    public:
        Tank(Point pos, Direction dir, int spd, int hp, int fireRate) 
            : GameObject(pos, dir, spd, hp, true), 
              fireRate(fireRate), 
              reloadTime(fireRate), // время перезарядки = скорострельность
              currentReload(0),
              hasShield(false),
              doubleFire(false),
              speedBoost(false),
              speedBoostDuration(0)
        {}
        
        // Метод выстрела - мгновенная проверка попадания
        virtual Projectile* fire(GameWorld& world) {
            if (!canFire()) return nullptr;
            
            currentReload = reloadTime;
            
            // Создаем снаряд для расчета траектории
            Point projectilePos = position;
            int range = 10; // дальность снаряда
            
            // Проверяем линию огня до первого препятствия или врага
            Point checkPos = position;
            GameObject* hitTarget = nullptr;
            
            for (int i = 0; i < range; i++) {
                // Двигаемся в направлении выстрела
                switch (direction) {
                    case Direction::UP:
                        checkPos.y -= 1;
                        break;
                    case Direction::DOWN:
                        checkPos.y += 1;
                        break;
                    case Direction::LEFT:
                        checkPos.x -= 1;
                        break;
                    case Direction::RIGHT:
                        checkPos.x += 1;
                        break;
                }
                
                // Проверяем выход за границы
                if (checkPos.x < 0 || checkPos.x >= world.getWidth() || 
                    checkPos.y < 0 || checkPos.y >= world.getHeight()) {
                    break;
                }
                
                // Ищем объекты в текущей позиции
                auto objectsInCell = world.getObjectsInRadius(checkPos, 0);
                for (auto* obj : objectsInCell) {
                    if (obj != this && obj->isDestroyed() == false) {
                        // Проверяем, можно ли поразить объект
                        if (dynamic_cast<Obstacle*>(obj)) {
                            Obstacle* obstacle = static_cast<Obstacle*>(obj);
                            if (!obstacle->isPassable()) {
                                hitTarget = obj;
                                break;
                            }
                        } else if (dynamic_cast<Tank*>(obj)) {
                            Tank* tank = static_cast<Tank*>(obj);
                            hitTarget = obj;
                            break;
                        }
                    }
                }
                
                if (hitTarget) break;
            }
            
            // Если есть цель, наносим урон
            if (hitTarget) {
                hitTarget->takeDamage(1);
            }
            
            // Создаем визуальный снаряд (для отрисовки траектории)
            Projectile* projectile = new Projectile(position, direction, 1, this);
            
            // Если активирован двойной выстрел, создаем второй выстрел
            if (doubleFire) {
                // Второй выстрел проверяет параллельную линию
                Point secondCheckPos = position;
                GameObject* secondHitTarget = nullptr;
                
                for (int i = 0; i < range; i++) {
                    switch (direction) {
                        case Direction::UP:
                        case Direction::DOWN:
                            secondCheckPos.x += (i % 2 == 0) ? 1 : -1; // зигзаг
                            secondCheckPos.y = position.y + ((direction == Direction::UP) ? -i : i);
                            break;
                        case Direction::LEFT:
                        case Direction::RIGHT:
                            secondCheckPos.y += (i % 2 == 0) ? 1 : -1; // зигзаг
                            secondCheckPos.x = position.x + ((direction == Direction::LEFT) ? -i : i);
                            break;
                    }
                    
                    // Проверяем границы и объекты аналогично первому выстрелу
                    if (secondCheckPos.x < 0 || secondCheckPos.x >= world.getWidth() || 
                        secondCheckPos.y < 0 || secondCheckPos.y >= world.getHeight()) {
                        break;
                    }
                    
                    auto objectsInCell = world.getObjectsInRadius(secondCheckPos, 0);
                    for (auto* obj : objectsInCell) {
                        if (obj != this && obj != hitTarget && obj->isDestroyed() == false) {
                            if (dynamic_cast<Obstacle*>(obj)) {
                                Obstacle* obstacle = static_cast<Obstacle*>(obj);
                                if (!obstacle->isPassable()) {
                                    secondHitTarget = obj;
                                    break;
                                }
                            } else if (dynamic_cast<Tank*>(obj)) {
                                Tank* tank = static_cast<Tank*>(obj);
                                secondHitTarget = obj;
                                break;
                            }
                        }
                    }
                    
                    if (secondHitTarget) break;
                }
                
                if (secondHitTarget) {
                    secondHitTarget->takeDamage(1);
                }
            }
            
            return projectile;
        }
        
        void applyBonus(BonusType bonus) {
            switch (bonus) {
                case BonusType::SHIELD:
                    hasShield = true;
                    break;
                case BonusType::DOUBLE_FIRE:
                    doubleFire = true;
                    break;
                case BonusType::SPEED_BOOST:
                    speedBoost = true;
                    speedBoostDuration = 10; // длительность 10 ходов
                    speed *= 2;
                    break;
                case BonusType::LIFE_UP:
                    health += 1;
                    break;
            }
        }
        
        void reload() {
            if (currentReload > 0) {
                currentReload--;
            }
            
            // Обновляем длительность бонусов
            if (speedBoost && speedBoostDuration > 0) {
                speedBoostDuration--;
                if (speedBoostDuration == 0) {
                    speedBoost = false;
                    speed /= 2; // возвращаем нормальную скорость
                }
            }
        }
        
        bool canFire() const {
            return currentReload == 0 && health > 0;
        }
        
        // Переопределяем метод получения урона для учета щита
        void takeDamage(int damage) override {
            if (hasShield) {
                hasShield = false; // Щит поглощает урон и исчезает
                return;
            }
            GameObject::takeDamage(damage);
        }
        
        // Переопределяем метод движения для пошагового геймплея
        void move(Direction newDirection, GameWorld& world) {
            if (isDestroyed()) return;
            
            // Поворачиваем танк в направлении движения
            rotate(newDirection);
            
            Point newPos = position;
            
            // Рассчитываем новую позицию
            switch (direction) {
                case Direction::UP:
                    newPos.y -= 1;
                    break;
                case Direction::DOWN:
                    newPos.y += 1;
                    break;
                case Direction::LEFT:
                    newPos.x -= 1;
                    break;
                case Direction::RIGHT:
                    newPos.x += 1;
                    break;
            }
            
            // Проверяем границы
            if (newPos.x < 0 || newPos.x >= world.getWidth() || 
                newPos.y < 0 || newPos.y >= world.getHeight()) {
                return;
            }
            
            // Проверяем столкновения с непроходимыми объектами
            auto objectsAtNewPos = world.getObjectsInRadius(newPos, 0);
            bool canMove = true;
            
            for (auto* obj : objectsAtNewPos) {
                if (obj != this && !obj->isDestroyed()) {
                    if (dynamic_cast<Obstacle*>(obj)) {
                        Obstacle* obstacle = static_cast<Obstacle*>(obj);
                        if (!obstacle->isPassable()) {
                            canMove = false;
                            break;
                        }
                    } else if (dynamic_cast<Tank*>(obj)) {
                        // Не можем двигаться через другие танки
                        canMove = false;
                        break;
                    }
                }
            }
            
            if (canMove) {
                position = newPos;
            }
        }
        
        // Геттеры
        bool getHasShield() const { return hasShield; }
        bool getDoubleFire() const { return doubleFire; }
        bool getSpeedBoost() const { return speedBoost; }
        int getCurrentReload() const { return currentReload; }
        
        // Абстрактные методы
        virtual void update() override = 0;
        virtual Point getBounds() const override = 0;
        virtual char getSymbol() const override = 0;
};

#endif // TANK_CPP