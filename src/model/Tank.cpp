// src/model/Tank.cpp
#ifndef TANK_CPP
#define TANK_CPP

#include "GameObject.cpp"
#include "Projectile.cpp"
#include "Bonus.cpp"

class Tank : public GameObject {
    protected:
        int fireRate;
        int reloadTime;
        int currentReload;
        bool hasShield;
        bool doubleFire;
        int speedBoost;
        int bonusDuration;

    public:
        Tank(Point pos, Direction dir, int spd, int hp, int fireRate)
            : GameObject(pos, dir, spd, hp, true), 
              fireRate(fireRate), reloadTime(0), currentReload(0),
              hasShield(false), doubleFire(false), speedBoost(0), bonusDuration(0) {}
        
        virtual ~Tank() = default;
        
        // Методы танка
        virtual Projectile* fire() {
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
            
            // Создаем снаряд
            return new Projectile(projectilePos, direction, 1, this);
        }
        
        void applyBonus(BonusType bonus) {
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
        
        void updateBonus() {
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
        
        void reload() {
            if (currentReload > 0) {
                currentReload--;
            }
        }
        
        bool canFire() const {
            return currentReload == 0;
        }
        
        // Переопределение метода получения урона с учетом щита
        void takeDamage(int damage) override {
            if (hasShield) {
                // Щит поглощает урон
                return;
            }
            
            GameObject::takeDamage(damage);
        }
        
        // Переопределение метода движения с учетом ускорения
        void move(Direction dir) override {
            // Сохраняем предыдущую позицию на случай столкновения
            Point prevPosition = position;
            
            // Обновляем направление
            direction = dir;
            
            // Вычисляем фактическую скорость с учетом бонуса
            int actualSpeed = speed + speedBoost;
            
            // Вычисляем новую позицию в зависимости от направления
            switch (dir) {
                case Direction::UP:
                    position.y -= actualSpeed;
                    break;
                case Direction::DOWN:
                    position.y += actualSpeed;
                    break;
                case Direction::LEFT:
                    position.x -= actualSpeed;
                    break;
                case Direction::RIGHT:
                    position.x += actualSpeed;
                    break;
            }
        }
        
        // Геттеры
        bool getHasShield() const { return hasShield; }
        bool getDoubleFire() const { return doubleFire; }
        int getCurrentReload() const { return currentReload; }
        int getBonusDuration() const { return bonusDuration; }
        
        // Сеттеры для настройки параметров
        void setReloadTime(int reload) { reloadTime = reload; }
        void setFireRate(int rate) { fireRate = rate; }
        
        // Абстрактные методы
        virtual void update() override = 0;
        virtual Point getBounds() const override = 0;
        virtual char getSymbol() const override = 0;
};

#endif // TANK_CPP