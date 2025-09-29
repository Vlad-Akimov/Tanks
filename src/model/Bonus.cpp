#ifndef BONUS_CPP
#define BONUS_CPP

#include "GameObject.cpp"

// Перенесем объявление BonusType сюда
enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };

class Tank; // Forward declaration

class Bonus : public GameObject {
    private:
        BonusType type;
        int duration;
        bool active;

    public:
        Bonus(Point pos, BonusType bonusType, int dur = 0);
        
        // Методы бонуса
        void applyEffect(Tank* tank);
        void deactivate();
        bool isActive() const { return active; }
        BonusType getType() const { return type; }
        
        // Реализация абстрактных методов
        void update() override;
        Point getBounds() const override;
        char getSymbol() const override;
};

#endif // BONUS_CPP