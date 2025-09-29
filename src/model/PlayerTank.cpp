#ifndef PLAYERTANK_CPP
#define PLAYERTANK_CPP

#include "Tank.cpp"

class PlayerTank : public Tank {
    private:
        int lives;
        int score;

    public:
        PlayerTank(Point pos);
        
        // Методы игрока
        void addScore(int points);
        void loseLife();
        bool hasLives() const;
        
        // Геттеры
        int getLives() const { return lives; }
        int getScore() const { return score; }
        
        // Реализация абстрактных методов
        void update() override;
        Point getBounds() const override;
        char getSymbol() const override;
};

#endif // PLAYERTANK_CPP