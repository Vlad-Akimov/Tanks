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
        bool hasShield;
        bool doubleFire;

    public:
        Tank(Point pos, Direction dir, int spd, int hp, int fireRate);
        virtual ~Tank() = default;
        
        // Методы танка
        virtual Projectile* fire();
        void applyBonus(BonusType bonus);
        void reload();
        bool canFire() const;
        
        // Геттеры
        bool getHasShield() const { return hasShield; }
        bool getDoubleFire() const { return doubleFire; }
        
        // Абстрактные методы
        virtual void update() override = 0;
        virtual Point getBounds() const override = 0;
        virtual char getSymbol() const override = 0;
};

#endif // TANK_CPP