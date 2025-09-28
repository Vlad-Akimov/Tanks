// src/model/GameWorld.cpp
#ifndef GAMEWORLD_CPP
#define GAMEWORLD_CPP

#include <vector>
#include <memory>
#include "GameObject.cpp"
#include "PlayerTank.cpp"
#include "EnemyTank.cpp"
#include "Obstacle.cpp"
#include "Bonus.cpp"
#include "Projectile.cpp"

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, SETTINGS };

class GameWorld {
    private:
        std::vector<std::unique_ptr<GameObject>> objects;
        std::vector<std::unique_ptr<Bonus>> bonuses;
        std::vector<std::unique_ptr<Projectile>> projectiles;
        GameState state;
        int currentLevel;
        int fieldWidth, fieldHeight;
        PlayerTank* player;

    public:
        GameWorld(int width, int height);
        
        // Основные методы игры
        void update();
        void loadLevel(int level);
        void checkCollisions();
        void spawnBonus();
        std::vector<GameObject*> GameWorld::getObjectsInRadius(Point center, int radius) const {
        std::vector<GameObject*> result;
        
        for (const auto& obj : objects) {
            if (!obj->isDestroyed()) {
                int dx = obj->getPosition().x - center.x;
                int dy = obj->getPosition().y - center.y;
                int distanceSquared = dx * dx + dy * dy;
                
                if (distanceSquared <= radius * radius) {
                    result.push_back(obj.get());
                }
            }
        }
        
        return result;
    }
        
        // Управление состоянием игры
        void setState(GameState newState);
        GameState getState() const { return state; }
        
        // Геттеры
        int getWidth() const { return fieldWidth; }
        int getHeight() const { return fieldHeight; }
        PlayerTank* getPlayer() const { return player; }
        const auto& getObjects() const { return objects; }
        const auto& getProjectiles() const { return projectiles; }
        
        // Добавление объектов
        void addObject(std::unique_ptr<GameObject> obj);
        void addProjectile(std::unique_ptr<Projectile> proj);
        void addBonus(std::unique_ptr<Bonus> bonus);
};

#endif // GAMEWORLD_CPP