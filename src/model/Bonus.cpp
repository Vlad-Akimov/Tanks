#ifndef BONUS_CPP
#define BONUS_CPP

#include "GameObject.cpp"

enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };

class Tank; // Forward declaration

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
        void applyEffect(Tank* tank);
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

void Bonus::applyEffect(Tank* tank) {
    if (!tank || !active) return;
    
    switch(type) {
        case BonusType::SHIELD:
            // Устанавливаем щит на танк
            // В пошаговой игре щит действует определенное количество ходов
            tank->applyBonus(BonusType::SHIELD);
            break;
            
        case BonusType::DOUBLE_FIRE:
            // Активируем двойной огонь
            tank->applyBonus(BonusType::DOUBLE_FIRE);
            break;
            
        case BonusType::SPEED_BOOST:
            // Увеличиваем скорость танка
            tank->applyBonus(BonusType::SPEED_BOOST);
            break;
            
        case BonusType::LIFE_UP:
            // Добавляем жизнь (только для PlayerTank)
            if (auto* playerTank = dynamic_cast<PlayerTank*>(tank)) {
                // В пошаговой игре добавляем одну жизнь
                // В реальной реализации нужно добавить метод addLife()
                playerTank->setHealth(playerTank->getHealth() + 1);
            }
            break;
    }
    
    // Деактивируем бонус после применения
    deactivate();
}

#endif // BONUS_CPP