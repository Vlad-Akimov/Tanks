#ifndef BONUS_CPP
#define BONUS_CPP

#include "GameObject.cpp"

class PlayerTank;

enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };

class Bonus : public GameObject {
    private:
        BonusType type;
        int duration;
        bool active;
        int activationTime;

    public:
        Bonus(Point pos, BonusType bonusType, int dur = 0)
            : GameObject(pos, Direction::UP, 0, 1, true), 
              type(bonusType), duration(dur), active(true), activationTime(0) {}
        
        // Методы бонуса
        void applyEffect(PlayerTank* playerTank);
        
        void deactivate() { active = false; }
        bool isActive() const { return active; }
        BonusType getType() const { return type; }
        
        // Реализация абстрактных методов
        void update() override {
            // В пошаговой игре бонус не требует постоянного обновления
            // Время действия обрабатывается в GameWorld
        }
        
        Point getBounds() const override {
            // Бонус занимает одну клетку
            return Point(1, 1);
        }
        
        char getSymbol() const override {
            switch(type) {
                case BonusType::SHIELD: return 'S';
                case BonusType::DOUBLE_FIRE: return 'F';
                case BonusType::SPEED_BOOST: return 'B';
                case BonusType::LIFE_UP: return 'L';
                default: return '?';
            }
        }
};

#endif // BONUS_CPP