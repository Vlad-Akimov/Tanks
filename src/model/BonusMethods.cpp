#include "Bonus.cpp"
#include "PlayerTank.cpp"

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