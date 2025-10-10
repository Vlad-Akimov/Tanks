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
    int fieldWidth, fieldHeight;
    GameState state;
    int currentLevel;
    PlayerTank* player;
    int enemyCount;
    int maxEnemies;

    // Структура для параметров сложности
    struct DifficultyParams {
        int brickCount;
        int steelCount;
        int waterCount;
        int forestCount;
        int brickClusters;
        int steelClusters;
        int waterClusters;
        int forestClusters;
        bool organicClusters;
    };
    
    // Метод для настройки параметров сложности
    DifficultyParams adjustDifficulty(int level) {
        DifficultyParams params;
        
        // Базовые значения для уровня 1
        int baseObstacles = 60;
        
        // Прогрессия количества препятствий
        params.brickCount = 40 + level * 8;
        params.steelCount = 15 + level * 4;
        params.waterCount = 25 + level * 6;
        params.forestCount = 20 + level * 5;
        
        // Прогрессия количества кластеров
        params.brickClusters = 3 + level / 2;
        params.steelClusters = std::max(0, (level - 2) / 2); // Сталь появляется с 3 уровня
        params.waterClusters = 2 + level / 3;
        params.forestClusters = 2 + level / 3;
        
        // Органичные кластеры включаются с 2 уровня
        params.organicClusters = (level >= 2);
        
        // Корректировка для очень высоких уровней
        if (level >= 8) {
            params.steelCount += 10;
            params.brickCount -= 5; // Заменяем часть кирпича на сталь
        }
        
        // Ограничение максимального количества препятствий
        int totalObstacles = params.brickCount + params.steelCount + params.waterCount + params.forestCount;
        int maxObstacles = 200; // Максимум чтобы поле не было переполнено
        
        if (totalObstacles > maxObstacles) {
            float scale = static_cast<float>(maxObstacles) / totalObstacles;
            params.brickCount = static_cast<int>(params.brickCount * scale);
            params.steelCount = static_cast<int>(params.steelCount * scale);
            params.waterCount = static_cast<int>(params.waterCount * scale);
            params.forestCount = static_cast<int>(params.forestCount * scale);
        }
        
        return params;
    }

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
            if (!obj->isDestroyed()) {
                obj->update(); 
            }
        }
        
        // ОБРАБОТКА ВЫСТРЕЛОВ ВРАЖЕСКИХ ТАНКОВ
        for (auto& obj : objects) {
            EnemyTank* enemy = dynamic_cast<EnemyTank*>(obj.get());
            if (enemy && !enemy->isDestroyed()) {
                Projectile* projectile = enemy->fire();
                if (projectile) {
                    projectiles.emplace_back(projectile);
                }
            }
        }
        
        // Обновляем бонусы
        for (auto& bonus : bonuses) { 
            if (bonus->isActive()) {
                bonus->update(); 
            }
        }
        
        // Проверяем столкновения
        checkCollisions();
        
        // Удаляем уничтоженные объекты
        cleanupDestroyedObjects();
        
        // Спауним бонусы с случайным шансом
        if (rand() % 100 < 5) { 
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
        
        // Полностью сбрасываем состояние игрока при загрузке нового уровня
        if (player) {
            player->reset();
            player->setPosition(Point(fieldWidth / 2, fieldHeight - 3));
        }
        
        // Создаем препятствия в зависимости от уровня
        createLevelObstacles(level);
        
        // Создаем врагов в зависимости от уровня
        createEnemies(level);
        
        state = GameState::PLAYING;
    }
    
    void checkCollisions() {
        if (!player) return;
        
        // Обрабатываем снаряды
        for (auto& projectile : projectiles) {
            if (!projectile || projectile->isDestroyed() || projectile->isProcessed()) continue;
            
            Tank* owner = projectile->getOwner();
            int damage = projectile->getDamage();
            Direction dir = projectile->getDirection();
            
            // Получаем всю траекторию снаряда
            auto trajectory = projectile->getTrajectory();
            bool hit = false;
            
            // Проверяем каждую точку траектории
            for (const auto& point : trajectory) {
                // Проверяем выход за границы поля
                if (point.x < 0 || point.x >= fieldWidth || point.y < 0 || point.y >= fieldHeight) {
                    hit = true;
                    break;
                }
                
                // Проверяем столкновение с объектами (кроме владельца снаряда)
                for (auto& obj : objects) {
                    if (!obj || obj->isDestroyed() || obj.get() == owner) continue;
                    
                    Point objPos = obj->getPosition();
                    Point objBounds = obj->getBounds();
                    
                    // Проверяем попадание в объект
                    if (point.x >= objPos.x && point.x < objPos.x + objBounds.x &&
                        point.y >= objPos.y && point.y < objPos.y + objBounds.y) {
                        
                        // Проверяем, является ли объект препятствием
                        Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
                        if (obstacle) {
                            // Если препятствие проходимо для снарядов - пропускаем
                            if (obstacle->isProjectilePassable()) {
                                continue; // Продолжаем проверять другие объекты в этой точке
                            }
                            
                            // Если препятствие НЕ проходимо - наносим урон и останавливаем снаряд
                            if (obstacle->isDestructible()) {
                                obstacle->takeDamage(damage);
                                
                                // Начисляем очки за разрушение препятствия
                                if (owner == player) {
                                    player->addScore(10);
                                }
                            }
                            hit = true;
                            break;
                        }
                        
                        // Если это танк (игрок или враг)
                        Tank* tank = dynamic_cast<Tank*>(obj.get());
                        if (tank) {
                            // Наносим урон танку
                            tank->takeDamage(damage);
                            
                            // Начисляем очки игроку за уничтожение врага
                            if (tank != player && tank->isDestroyed() && owner == player) {
                                player->addScore(100);
                            }
                            hit = true;
                            break;
                        }
                        
                        // Для других типов объектов (бонусов и т.д.) снаряд проходит насквозь
                        Bonus* bonus = dynamic_cast<Bonus*>(obj.get());
                        if (bonus) {
                            continue; // Снаряд проходит сквозь бонусы
                        }
                    }
                }
                
                if (hit) break;
            }
            
            // Помечаем снаряд как обработанный
            projectile->markProcessed();
        }
        
        // Проверяем сбор бонусов игроком
        Point playerPos = player->getPosition();
        Point playerBounds = player->getBounds();
        
        for (auto& bonus : bonuses) {
            if (!bonus || !bonus->isActive()) continue;
            
            Point bonusPos = bonus->getPosition();
            
            if (playerPos.x <= bonusPos.x && playerPos.x + playerBounds.x > bonusPos.x &&
                playerPos.y <= bonusPos.y && playerPos.y + playerBounds.y > bonusPos.y) {
                
                bonus->applyEffect(player);
                bonus->deactivate();
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
                
                if (tankPos.x < obsPos.x + obsBounds.x && tankPos.x + tankBounds.x > obsPos.x &&
                    tankPos.y < obsPos.y + obsBounds.y && tankPos.y + tankBounds.y > obsPos.y) {
                    
                    if (!obs->isPassable()) {
                        Direction dir = tank->getDirection();
                        Point prevPos = tankPos;
                        
                        // Для скорости больше 1 нужно откатывать на соответствующее расстояние
                        int actualSpeed = tank->getSpeed();
                        PlayerTank* playerTank = dynamic_cast<PlayerTank*>(tank);
                        if (playerTank && playerTank->getBonusDuration() > 0) {
                            actualSpeed += 1; // Учитываем бонус скорости
                        }
                        
                        switch (dir) {
                            case Direction::UP:
                                tank->setPosition(Point(prevPos.x, prevPos.y + actualSpeed));
                                break;
                            case Direction::DOWN:
                                tank->setPosition(Point(prevPos.x, prevPos.y - actualSpeed));
                                break;
                            case Direction::LEFT:
                                tank->setPosition(Point(prevPos.x + actualSpeed, prevPos.y));
                                break;
                            case Direction::RIGHT:
                                tank->setPosition(Point(prevPos.x - actualSpeed, prevPos.y));
                                break;
                        }
                    }
                }
            }
        }
        
        // Проверяем столкновения танков друг с другом
        for (auto& obj1 : objects) {
            Tank* tank1 = dynamic_cast<Tank*>(obj1.get());
            if (!tank1 || tank1->isDestroyed()) continue;
            
            Point tank1Pos = tank1->getPosition();
            Point tank1Bounds = tank1->getBounds();
            
            for (auto& obj2 : objects) {
                if (obj1.get() == obj2.get()) continue;
                
                Tank* tank2 = dynamic_cast<Tank*>(obj2.get());
                if (!tank2 || tank2->isDestroyed()) continue;
                
                Point tank2Pos = tank2->getPosition();
                Point tank2Bounds = tank2->getBounds();
                
                // Проверяем пересечение
                if (tank1Pos.x < tank2Pos.x + tank2Bounds.x &&
                    tank1Pos.x + tank1Bounds.x > tank2Pos.x &&
                    tank1Pos.y < tank2Pos.y + tank2Bounds.y &&
                    tank1Pos.y + tank1Bounds.y > tank2Pos.y) {
                    
                    // Откатываем позицию первого танка
                    Direction dir = tank1->getDirection();
                    Point prevPos = tank1Pos;
                    
                    // Учитываем скорость для отката
                    int actualSpeed = tank1->getSpeed();
                    PlayerTank* playerTank1 = dynamic_cast<PlayerTank*>(tank1);
                    if (playerTank1 && playerTank1->getBonusDuration() > 0) {
                        actualSpeed += 1;
                    }
                    
                    switch (dir) {
                        case Direction::UP:
                            tank1->setPosition(Point(prevPos.x, prevPos.y + actualSpeed));
                            break;
                        case Direction::DOWN:
                            tank1->setPosition(Point(prevPos.x, prevPos.y - actualSpeed));
                            break;
                        case Direction::LEFT:
                            tank1->setPosition(Point(prevPos.x + actualSpeed, prevPos.y));
                            break;
                        case Direction::RIGHT:
                            tank1->setPosition(Point(prevPos.x - actualSpeed, prevPos.y));
                            break;
                    }
                    
                    // Выходим из внутреннего цикла после обработки столкновения
                    break;
                }
            }
        }
        
        // Проверяем столкновения танков с границами поля
        for (auto& obj : objects) {
            Tank* tank = dynamic_cast<Tank*>(obj.get());
            if (!tank || tank->isDestroyed()) continue;
            
            Point tankPos = tank->getPosition();
            Point tankBounds = tank->getBounds();
            
            // Проверяем выход за границы и корректируем позицию
            if (tankPos.x < 0) tank->setPosition(Point(0, tankPos.y));
            if (tankPos.y < 0) tank->setPosition(Point(tankPos.x, 0));
            if (tankPos.x + tankBounds.x >= fieldWidth) 
                tank->setPosition(Point(fieldWidth - tankBounds.x, tankPos.y));
            if (tankPos.y + tankBounds.y >= fieldHeight) 
                tank->setPosition(Point(tankPos.x, fieldHeight - tankBounds.y));
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
            
            if (distance <= radius) { result.push_back(obj.get()); }
        }
        
        return result;
    }
    
    // Управление состоянием игры
    void setState(GameState newState) { state = newState; }
    
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
        if (dynamic_cast<EnemyTank*>(objects.back().get())) { enemyCount++; }
    }
    
    void addProjectile(std::unique_ptr<Projectile> proj) { projectiles.push_back(std::move(proj)); }
    
    void addBonus(std::unique_ptr<Bonus> bonus) { bonuses.push_back(std::move(bonus)); }
    
    // Метод для обработки выстрела игрока
    void playerFire() {
        if (state != GameState::PLAYING) return;
        
        Projectile* projectile = player->fire();
        if (projectile) { projectiles.emplace_back(projectile); }
    }
    
    // Метод для движения игрока
    void playerMove(Direction dir) {
        if (state != GameState::PLAYING) return;
        
        // Сохраняем предыдущую позицию
        Point prevPos = player->getPosition();
        Point bounds = player->getBounds();
        
        player->rotate(dir);
        player->move(dir);
        
        Point newPos = player->getPosition();
        
        // Проверяем валидность новой позиции
        if (!isValidPosition(newPos, bounds, player)) {
            // Если позиция невалидна - возвращаем на предыдущую
            player->setPosition(prevPos);
        }
        
        // Проверяем выход за границы поля (дополнительная проверка)
        if (newPos.x < 0) player->setPosition(Point(0, newPos.y));
        if (newPos.y < 0) player->setPosition(Point(newPos.x, 0));
        if (newPos.x + bounds.x >= fieldWidth) player->setPosition(Point(fieldWidth - bounds.x, newPos.y));
        if (newPos.y + bounds.y >= fieldHeight) player->setPosition(Point(newPos.x, fieldHeight - bounds.y));
    }

private:
    bool isValidPosition(const Point& pos, const Point& bounds, const GameObject* excludeObj = nullptr) const {
        // Проверяем границы поля
        if (pos.x < 0 || pos.y < 0 || 
            pos.x + bounds.x > fieldWidth || 
            pos.y + bounds.y > fieldHeight) {
            return false;
        }
        
        // Проверяем столкновения с объектами
        for (const auto& obj : objects) {
            if (!obj || obj->isDestroyed() || obj.get() == excludeObj) continue;
            
            Point objPos = obj->getPosition();
            Point objBounds = obj->getBounds();
            
            // Проверяем пересечение
            if (pos.x < objPos.x + objBounds.x &&
                pos.x + bounds.x > objPos.x &&
                pos.y < objPos.y + objBounds.y &&
                pos.y + bounds.y > objPos.y) {
                
                // Если это непроходимое препятствие - позиция невалидна
                Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
                if (obstacle && !obstacle->isPassable()) {
                    return false;
                }
                
                // Если это другой танк - позиция невалидна
                if (dynamic_cast<Tank*>(obj.get())) {
                    return false;
                }
            }
        }
        
        return true;
    }

    void createLevelObstacles(int level);
    
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
            }
        }
    }
    
    void cleanupDestroyedObjects() {
        // Удаляем уничтоженные объекты, но НЕ игрока
        objects.erase(std::remove_if(objects.begin(), objects.end(),
            [this](const std::unique_ptr<GameObject>& obj) {
                return obj->isDestroyed() && obj.get() != player; // Исключаем игрока
            }), objects.end());
        
        // Обновляем счетчик врагов
        enemyCount = 0;
        for (const auto& obj : objects) {
            if (dynamic_cast<EnemyTank*>(obj.get())) {
                enemyCount++;
            }
        }
        
        // Удаляем обработанные или уничтоженные снаряды
        projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& proj) {
                return proj->isDestroyed() || proj->isProcessed();
            }), projectiles.end());
        
        // Удаляем неактивные бонусы
        bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(),
            [](const std::unique_ptr<Bonus>& bonus) {
                return !bonus->isActive();
            }), bonuses.end());
    }
    
    void checkGameConditions() {
        // Проверяем, что player валиден
        if (!player) return;
        
        // Проверяем смерть игрока
        if (player->isDestroyed()) {
            player->loseLife();
            
            if (player && player->hasLives()) {
                // Респавн игрока
                player->setPosition(Point(fieldWidth / 2, fieldHeight - 3));
                player->setHealth(3);
                player->setDestroyed(false);
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