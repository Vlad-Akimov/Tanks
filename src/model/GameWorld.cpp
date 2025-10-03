#ifndef GAMEWORLD_CPP
#define GAMEWORLD_CPP

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
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
    int enemyCount;
    int maxEnemies;

public:
    GameWorld(int width, int height) 
        : fieldWidth(width), fieldHeight(height), state(GameState::MENU), 
          currentLevel(1), player(nullptr), enemyCount(0), maxEnemies(5) {
        // Создаем игрока в центре нижней части поля
        Point playerPos(width / 2, height - 3);
        player = new PlayerTank(playerPos);
        objects.emplace_back(player);
    }
    
    // Основные методы игры
    void update() {
        if (state != GameState::PLAYING) return;
        
        // Обновляем все объекты
        for (auto& obj : objects) {
            obj->update();
        }
        
        // Обновляем бонусы
        for (auto& bonus : bonuses) {
            bonus->update();
        }
        
        // Проверяем столкновения
        checkCollisions();
        
        // Удаляем уничтоженные объекты
        cleanupDestroyedObjects();
        
        // Спауним бонусы с случайным шансом
        if (rand() % 100 < 5) { // 5% шанс каждый ход
            spawnBonus();
        }
        
        // Обновляем позиции игрока для врагов
        updateEnemyAI();
        
        // Проверяем условия завершения уровня/игры
        checkGameConditions();
    }
    
    void loadLevel(int level) {
        currentLevel = level;
        
        // Очищаем поле от предыдущих объектов (кроме игрока)
        objects.erase(std::remove_if(objects.begin(), objects.end(), 
            [this](const std::unique_ptr<GameObject>& obj) {
                return obj.get() != player;
            }), objects.end());
        
        bonuses.clear();
        projectiles.clear();
        
        // Устанавливаем позицию игрока
        player->setPosition(Point(fieldWidth / 2, fieldHeight - 3));
        player->setHealth(3);
        
        // Создаем препятствия в зависимости от уровня
        createLevelObstacles(level);
        
        // Создаем врагов в зависимости от уровня
        createEnemies(level);
        
        state = GameState::PLAYING;
    }
    
    void checkCollisions() {
        // Проверяем столкновения снарядов с объектами
        for (auto& projectile : projectiles) {
            if (!projectile || projectile->isDestroyed()) continue;
            
            Point projPos = projectile->getPosition();
            int damage = projectile->getDamage();
            
            // Проверяем столкновения с объектами
            for (auto& obj : objects) {
                if (!obj || obj->isDestroyed() || obj.get() == projectile->getOwner()) continue;
                
                Point objPos = obj->getPosition();
                Point objBounds = obj->getBounds();
                
                // Простая проверка прямоугольного пересечения
                if (projPos.x >= objPos.x && projPos.x < objPos.x + objBounds.x &&
                    projPos.y >= objPos.y && projPos.y < objPos.y + objBounds.y) {
                    
                    // Применяем урон к объекту
                    obj->takeDamage(damage);
                    
                    // Уничтожаем снаряд
                    projectile->takeDamage(1);
                    
                    // Начисляем очки игроку за уничтожение врага
                    if (obj.get() != player && obj->isDestroyed() && 
                        dynamic_cast<EnemyTank*>(obj.get())) {
                        player->addScore(100);
                    }
                    
                    break;
                }
            }
            
            // Проверяем выход за границы поля
            if (projPos.x < 0 || projPos.x >= fieldWidth || 
                projPos.y < 0 || projPos.y >= fieldHeight) {
                projectile->takeDamage(1);
            }
        }
        
        // Проверяем сбор бонусов игроком
        Point playerPos = player->getPosition();
        Point playerBounds = player->getBounds();
        
        for (auto& bonus : bonuses) {
            if (!bonus || !bonus->isActive()) continue;
            
            Point bonusPos = bonus->getPosition();
            
            if (playerPos.x <= bonusPos.x && playerPos.x + playerBounds.x > bonusPos.x &&
                playerPos.y <= bonusPos.y && playerPos.y + playerBounds.y > bonusPos.y) {
                
                // Применяем бонус к игроку
                bonus->applyEffect(player);
                bonus->deactivate();
                
                // Начисляем очки за сбор бонуса
                player->addScore(50);
            }
        }
        
        // Проверяем столкновения танков с препятствиями
        for (auto& obj : objects) {
            if (!obj || obj->isDestroyed()) continue;
            
            Tank* tank = dynamic_cast<Tank*>(obj.get());
            if (!tank) continue;
            
            Point tankPos = tank->getPosition();
            Point tankBounds = tank->getBounds();
            
            for (auto& obstacle : objects) {
                if (!obstacle || obstacle->isDestroyed() || obstacle.get() == obj.get()) continue;
                
                Obstacle* obs = dynamic_cast<Obstacle*>(obstacle.get());
                if (!obs) continue;
                
                Point obsPos = obstacle->getPosition();
                Point obsBounds = obstacle->getBounds();
                
                // Проверка пересечения
                if (tankPos.x < obsPos.x + obsBounds.x && tankPos.x + tankBounds.x > obsPos.x &&
                    tankPos.y < obsPos.y + obsBounds.y && tankPos.y + tankBounds.y > obsPos.y) {
                    
                    // Если препятствие непроходимо - откатываем позицию танка
                    if (!obs->isPassable()) {
                        // Простой откат к предыдущей позиции (в реальной реализации нужно хранить предыдущую позицию)
                        Direction dir = tank->getDirection();
                        switch (dir) {
                            case Direction::UP:
                                tank->setPosition(Point(tankPos.x, tankPos.y + 1));
                                break;
                            case Direction::DOWN:
                                tank->setPosition(Point(tankPos.x, tankPos.y - 1));
                                break;
                            case Direction::LEFT:
                                tank->setPosition(Point(tankPos.x + 1, tankPos.y));
                                break;
                            case Direction::RIGHT:
                                tank->setPosition(Point(tankPos.x - 1, tankPos.y));
                                break;
                        }
                    }
                }
            }
        }
    }
    
    void spawnBonus() {
        // Генерируем случайную позицию для бонуса
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xDist(1, fieldWidth - 2);
        std::uniform_int_distribution<> yDist(1, fieldHeight - 2);
        
        Point bonusPos(xDist(gen), yDist(gen));
        
        // Выбираем случайный тип бонуса
        std::uniform_int_distribution<> typeDist(0, 3);
        BonusType type = static_cast<BonusType>(typeDist(gen));
        
        // Создаем бонус
        auto bonus = std::make_unique<Bonus>(bonusPos, type);
        bonuses.push_back(std::move(bonus));
    }
    
    std::vector<GameObject*> getObjectsInRadius(Point center, int radius) const {
        std::vector<GameObject*> result;
        
        for (const auto& obj : objects) {
            if (!obj || obj->isDestroyed()) continue;
            
            Point objPos = obj->getPosition();
            int distance = abs(objPos.x - center.x) + abs(objPos.y - center.y);
            
            if (distance <= radius) {
                result.push_back(obj.get());
            }
        }
        
        return result;
    }
    
    // Управление состоянием игры
    void setState(GameState newState) {
        state = newState;
    }
    
    GameState getState() const { return state; }
    
    // Геттеры
    int getWidth() const { return fieldWidth; }
    int getHeight() const { return fieldHeight; }
    PlayerTank* getPlayer() const { return player; }
    const auto& getObjects() const { return objects; }
    const auto& getProjectiles() const { return projectiles; }
    const auto& getBonuses() const { return bonuses; }
    int getCurrentLevel() const { return currentLevel; }
    
    // Добавление объектов
    void addObject(std::unique_ptr<GameObject> obj) {
        objects.push_back(std::move(obj));
        
        // Обновляем счетчик врагов
        if (dynamic_cast<EnemyTank*>(objects.back().get())) {
            enemyCount++;
        }
    }
    
    void addProjectile(std::unique_ptr<Projectile> proj) {
        projectiles.push_back(std::move(proj));
    }
    
    void addBonus(std::unique_ptr<Bonus> bonus) {
        bonuses.push_back(std::move(bonus));
    }
    
    // Метод для обработки выстрела игрока
    void playerFire() {
        if (state != GameState::PLAYING) return;
        
        Projectile* projectile = player->fire();
        if (projectile) {
            projectiles.emplace_back(projectile);
        }
    }
    
    // Метод для движения игрока
    void playerMove(Direction dir) {
        if (state != GameState::PLAYING) return;
        
        player->rotate(dir);
        player->move(dir);
        
        // Проверяем выход за границы поля
        Point pos = player->getPosition();
        Point bounds = player->getBounds();
        
        if (pos.x < 0) player->setPosition(Point(0, pos.y));
        if (pos.y < 0) player->setPosition(Point(pos.x, 0));
        if (pos.x + bounds.x >= fieldWidth) player->setPosition(Point(fieldWidth - bounds.x, pos.y));
        if (pos.y + bounds.y >= fieldHeight) player->setPosition(Point(pos.x, fieldHeight - bounds.y));
    }

private:
    void createLevelObstacles(int level) {
        // Создаем границы поля
        for (int x = 0; x < fieldWidth; x++) {
            // Верхняя и нижняя границы
            objects.emplace_back(new Obstacle(Point(x, 0), ObstacleType::STEEL));
            objects.emplace_back(new Obstacle(Point(x, fieldHeight - 1), ObstacleType::STEEL));
        }
        
        for (int y = 1; y < fieldHeight - 1; y++) {
            // Левая и правая границы
            objects.emplace_back(new Obstacle(Point(0, y), ObstacleType::STEEL));
            objects.emplace_back(new Obstacle(Point(fieldWidth - 1, y), ObstacleType::STEEL));
        }
        
        // Создаем случайные препятствия в зависимости от уровня
        int obstacleCount = 10 + level * 3;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xDist(2, fieldWidth - 3);
        std::uniform_int_distribution<> yDist(2, fieldHeight - 3);
        std::uniform_int_distribution<> typeDist(0, 3);
        
        for (int i = 0; i < obstacleCount; i++) {
            Point pos(xDist(gen), yDist(gen));
            
            // Проверяем, чтобы препятствие не спаунилось на игроке
            Point playerPos = player->getPosition();
            if (abs(pos.x - playerPos.x) < 3 && abs(pos.y - playerPos.y) < 3) {
                continue;
            }
            
            ObstacleType type = static_cast<ObstacleType>(typeDist(gen));
            objects.emplace_back(new Obstacle(pos, type));
        }
        
        // Создаем базовые стены вокруг центра
        int centerX = fieldWidth / 2;
        int centerY = fieldHeight / 2;
        
        for (int x = centerX - 2; x <= centerX + 2; x++) {
            for (int y = centerY - 2; y <= centerY + 2; y++) {
                if (x == centerX && y == centerY) continue; // Оставляем проход в центре
                if ((x == centerX - 2 || x == centerX + 2) && (y == centerY - 2 || y == centerY + 2)) continue; // Углы
                
                objects.emplace_back(new Obstacle(Point(x, y), ObstacleType::BRICK));
            }
        }
    }
    
    void createEnemies(int level) {
        enemyCount = 0;
        maxEnemies = 3 + level;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xDist(2, fieldWidth - 4);
        std::uniform_int_distribution<> yDist(2, fieldHeight / 2); // Враги в верхней части
        std::uniform_int_distribution<> behaviorDist(0, 2);
        
        for (int i = 0; i < maxEnemies; i++) {
            Point pos(xDist(gen), yDist(gen));
            
            // Проверяем, чтобы враг не спаунился слишком близко к другим объектам
            bool validPosition = true;
            for (const auto& obj : objects) {
                if (!obj) continue;
                
                Point objPos = obj->getPosition();
                Point objBounds = obj->getBounds();
                
                if (pos.x >= objPos.x && pos.x < objPos.x + objBounds.x &&
                    pos.y >= objPos.y && pos.y < objPos.y + objBounds.y) {
                    validPosition = false;
                    break;
                }
            }
            
            if (!validPosition) continue;
            
            AIBehavior behavior = static_cast<AIBehavior>(behaviorDist(gen));
            int difficulty = std::min(3, level); // Сложность зависит от уровня
            
            auto enemy = std::make_unique<EnemyTank>(pos, behavior, difficulty);
            objects.push_back(std::move(enemy));
            enemyCount++;
        }
    }
    
    void updateEnemyAI() {
        Point playerPos = player->getPosition();
        
        for (auto& obj : objects) {
            EnemyTank* enemy = dynamic_cast<EnemyTank*>(obj.get());
            if (enemy && !enemy->isDestroyed()) {
                // Обновляем позицию игрока для ИИ врага
                enemy->setPlayerPosition(playerPos);
                
                // Враги автоматически стреляют в своем update()
                // Здесь мы можем добавить дополнительную логику если нужно
            }
        }
    }
    
    void cleanupDestroyedObjects() {
        // Удаляем уничтоженные объекты
        objects.erase(std::remove_if(objects.begin(), objects.end(),
            [](const std::unique_ptr<GameObject>& obj) {
                return obj->isDestroyed();
            }), objects.end());
        
        // Обновляем счетчик врагов
        enemyCount = 0;
        for (const auto& obj : objects) {
            if (dynamic_cast<EnemyTank*>(obj.get())) {
                enemyCount++;
            }
        }
        
        // Удаляем уничтоженные снаряды
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& proj) {
                return proj->isDestroyed();
            }), projectiles.end());
        
        // Удаляем неактивные бонусы
        bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(),
            [](const std::unique_ptr<Bonus>& bonus) {
                return !bonus->isActive();
            }), bonuses.end());
    }
    
    void checkGameConditions() {
        // Проверяем смерть игрока
        if (player->isDestroyed()) {
            player->loseLife();
            if (player->hasLives()) {
                // Респавн игрока
                player->setPosition(Point(fieldWidth / 2, fieldHeight - 3));
                player->setHealth(3);
            } else {
                state = GameState::GAME_OVER;
            }
        }
        
        // Проверяем завершение уровня (все враги уничтожены)
        if (enemyCount == 0) {
            currentLevel++;
            loadLevel(currentLevel);
        }
    }
};

#endif // GAMEWORLD_CPP