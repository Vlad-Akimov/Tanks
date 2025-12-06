#include "GameWorld.h"
#include <cstdlib>
#include <ctime>

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <cmath>
#include <map>

GameWorld::GameWorld(int width, int height) 
    : fieldWidth(40), fieldHeight(20), state(GameState::MENU), 
      currentLevel(1), player(nullptr), enemyCount(0), maxEnemies(5),
      damageFlashCounter(0) {
    // Создаем игрока в центре нижней части поля
    Point playerPos(width / 2, height - 3);
    player = new PlayerTank(playerPos);
    objects.emplace_back(player);
    
    // Инициализация случайного генератора
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void GameWorld::update() {
    if (state != GameState::PLAYING) return;

    // Обновляем счетчик эффекта урона
    if (damageFlashCounter > 0) {
        damageFlashCounter--;
    }
    
    updateEnemyAI();

    // Обновляем все объекты
    for (auto& obj : objects) { 
        if (!obj->isDestroyed()) {
            obj->update(); 
        }
    }

    for (auto& explosion : explosions) {
        explosion->update();
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
    if (std::rand() % 100 < 5) { 
        spawnBonus(); 
    }
    
    // Проверяем условия завершения уровня/игры
    checkGameConditions();
}

void GameWorld::loadLevel(int level) {
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
        player->reset(currentLevel);
        player->setPosition(Point(fieldWidth / 2, fieldHeight - 3));
    }
    
    // Создаем препятствия в зависимости от уровня
    createLevelObstacles(level);
    
    // Создаем врагов в зависимости от уровня
    createEnemies(level);
    
    state = GameState::PLAYING;
}

void GameWorld::checkCollisions() {
    if (!player) return;
    
    checkProjectileCollisions();
    checkBonusCollisions();
    checkTankObstacleCollisions();
    checkTankTankCollisions();
    checkBoundaryCollisions();
}

void GameWorld::checkProjectileCollisions() {
    // Обрабатываем снаряды
    for (auto& projectile : projectiles) {
        if (!projectile || projectile->isDestroyed() || projectile->isProcessed()) continue;
        
        Tank* owner = projectile->getOwner();
        int damage = projectile->getDamage();
        
        // Получаем всю траекторию снаряда
        auto trajectory = projectile->getTrajectory();
        bool hit = false;
        
        // Переменные для отслеживания предыдущей позиции
        Point lastValidPoint = projectile->getPosition();
        bool hasPreviousPoint = false;
        
        // Проверяем каждую точку траектории
        for (const auto& point : trajectory) {
            // Проверяем выход за границы поля
            if (point.x < 0 || point.x >= fieldWidth || point.y < 0 || point.y >= fieldHeight) {
                // Создаем взрыв на границе поля или последней валидной позиции
                Point explosionPoint = point;
                
                // Корректируем координаты для создания взрыва на границе
                if (point.x < 0) {
                    explosionPoint.x = 0;
                } else if (point.x >= fieldWidth) {
                    explosionPoint.x = fieldWidth - 1;
                }
                
                if (point.y < 0) {
                    explosionPoint.y = 0;
                } else if (point.y >= fieldHeight) {
                    explosionPoint.y = fieldHeight - 1;
                }
                
                // Если есть предыдущая валидная точка и она ближе к границе, используем ее
                if (hasPreviousPoint) {
                    // Проверяем, какая точка ближе к границе
                    int distLastToBorder = abs(lastValidPoint.x - explosionPoint.x) + 
                                          abs(lastValidPoint.y - explosionPoint.y);
                    int distCurrentToBorder = abs(point.x - explosionPoint.x) + 
                                             abs(point.y - explosionPoint.y);
                    
                    // Используем точку, которая ближе к границе
                    if (distLastToBorder < distCurrentToBorder) {
                        explosions.emplace_back(std::make_unique<Explosion>(lastValidPoint));
                    } else {
                        explosions.emplace_back(std::make_unique<Explosion>(explosionPoint));
                    }
                } else {
                    // Если первая точка уже за границей, создаем взрыв на границе
                    explosions.emplace_back(std::make_unique<Explosion>(explosionPoint));
                }
                hit = true;
                break;
            }
            
            // Сохраняем эту точку как последнюю валидную
            lastValidPoint = point;
            hasPreviousPoint = true;
            
            // Проверяем столкновение с объектами (кроме владельца снаряда)
            for (auto& obj : objects) {
                if (!obj || obj->isDestroyed() || obj.get() == owner) continue;
                
                Point objPos = obj->getPosition();
                Point objBounds = obj->getBounds();
                
                // Проверяем попадание в объект
                if (point.x >= objPos.x && point.x < objPos.x + objBounds.x &&
                    point.y >= objPos.y && point.y < objPos.y + objBounds.y) {
                    
                    // Обрабатываем попадание
                    if (handleProjectileHit(obj.get(), projectile.get(), damage)) {
                        hit = true;
                        break;
                    }
                }
            }
            
            if (hit) break;
        }
        
        // Помечаем снаряд как обработанный
        projectile->markProcessed();
    }
}

bool GameWorld::handleProjectileHit(GameObject* target, Projectile* projectile, int damage) {
    Tank* owner = projectile->getOwner();
    
    // Проверяем, является ли объект препятствием
    Obstacle* obstacle = dynamic_cast<Obstacle*>(target);
    if (obstacle) {
        // Если препятствие проходимо для снарядов - пропускаем
        if (obstacle->isProjectilePassable()) {
            return false; // Продолжаем проверять другие объекты в этой точке
        }
        
        // Если препятствие НЕ проходимо - наносим урон и останавливаем снаряд
        if (obstacle->isDestructible()) {
            bool wasDestroyed = obstacle->isDestroyed();
            obstacle->takeDamage(damage);
            
            if (!wasDestroyed && obstacle->isDestroyed()) {
                explosions.emplace_back(std::make_unique<Explosion>(obstacle->getPosition()));
            }

            // Начисляем очки за разрушение препятствия
            if (owner == player) {
                player->addScore(10);
            }
        }
        return true;
    }
    
    // Если это танк (игрок или враг)
    Tank* tank = dynamic_cast<Tank*>(target);
    if (tank) {
        EnemyTank* enemyOwner = dynamic_cast<EnemyTank*>(owner);
        EnemyTank* hitEnemy = dynamic_cast<EnemyTank*>(tank);
        
        // Игнорируем столкновения снарядов врагов с другими врагами
        if (enemyOwner && hitEnemy) {
            return false;
        }
        
        bool wasDestroyed = tank->isDestroyed();

        if (tank == player && !wasDestroyed) {
            triggerDamageFlash();
        }
        
        // Наносим урон танку
        tank->takeDamage(damage);
        
        if (!wasDestroyed && tank->isDestroyed()) {
            explosions.emplace_back(std::make_unique<Explosion>(tank->getPosition()));
        }

        // Начисляем очки игроку за уничтожение врага с бонусами за тип танка
        if (tank != player && tank->isDestroyed() && owner == player) {
            int baseScore = 100;
            
            // Бонусные очки за специальные типы танков
            if (hitEnemy) {
                switch(hitEnemy->getTankType()) {
                    case EnemyTankType::FAST:
                        baseScore = 150; // +50 очков за быстрый танк
                        break;
                    case EnemyTankType::DAMAGE:
                        baseScore = 200; // +100 очков за танк с уроном
                        break;
                    case EnemyTankType::ARMORED:
                        baseScore = 250; // +150 очков за бронированный танк
                        break;
                    case EnemyTankType::BASIC:
                    default:
                        baseScore = 100;
                        break;
                }
            }
            
            player->addScore(baseScore);
        }
        return true;
    }
    
    // Для других типов объектов (бонусов и т.д.) снаряд проходит насквозь
    Bonus* bonus = dynamic_cast<Bonus*>(target);
    if (bonus) {
        return false; // Снаряд проходит сквозь бонусы
    }
    
    return false;
}

void GameWorld::applySlowToEnemies(int duration) {
    for (auto& obj : objects) {
        EnemyTank* enemy = dynamic_cast<EnemyTank*>(obj.get());
        if (enemy && !enemy->isDestroyed()) {
            enemy->applySlowEffect(duration);
        }
    }
}

void GameWorld::checkBonusCollisions() {
    if (!player || player->isDestroyed()) return;
    
    // Проверяем сбор бонусов игроком
    Point playerPos = player->getPosition();
    
    for (auto& bonus : bonuses) {
        if (!bonus || !bonus->isActive()) continue;
        
        Point bonusPos = bonus->getPosition();
        
        if (playerPos.x == bonusPos.x && playerPos.y == bonusPos.y) {
            bonus->applyEffect(player);
            
            if (bonus->getType() == BonusType::SPEED_BOOST) {
                // Применяем замедление ко всем врагам на 20 ходов
                applySlowToEnemies(20);
            }
            
            player->addScore(50);
            bonus->deactivate();
        }
    }
}

void GameWorld::checkTankObstacleCollisions() {
    // Проверяем столкновения танков с препятствиями
    for (auto& obj : objects) {
        if (!obj || obj->isDestroyed()) continue;
        
        Tank* tank = dynamic_cast<Tank*>(obj.get());
        if (!tank) continue;
        
        Point tankPos = tank->getPosition();
        Point tankBounds = tank->getBounds();
        
        // Проверяем столкновение с каждым препятствием
        for (auto& obstacleObj : objects) {
            if (!obstacleObj || obstacleObj->isDestroyed() || obstacleObj.get() == obj.get()) continue;
            
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obstacleObj.get());
            if (!obstacle) continue;
            
            Point obsPos = obstacleObj->getPosition();
            Point obsBounds = obstacleObj->getBounds();
            
            // Проверяем пересечение
            if (tankPos.x < obsPos.x + obsBounds.x && 
                tankPos.x + tankBounds.x > obsPos.x &&
                tankPos.y < obsPos.y + obsBounds.y && 
                tankPos.y + tankBounds.y > obsPos.y) {
                
                if (!obstacle->isPassable()) {
                    // Откатываем танк на предыдущую позицию
                    handleTankObstacleCollision(tank);
                }
            }
        }
    }
}

void GameWorld::handleTankObstacleCollision(Tank* tank) {
    Point tankPos = tank->getPosition();
    Direction dir = tank->getDirection();
    
    // Простой откат на 1 клетку в противоположном направлении
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

void GameWorld::checkTankTankCollisions() {
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
                
                // Обрабатываем столкновение танков
                handleTankTankCollision(tank1);
                break;
            }
        }
    }
}

void GameWorld::handleTankTankCollision(Tank* tank1) {
    // Откатываем позицию первого танка
    Direction dir = tank1->getDirection();
    Point prevPos = tank1->getPosition();
    
    // Учитываем скорость для отката
    int actualSpeed = 1;
    
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
}

void GameWorld::checkBoundaryCollisions() {
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

bool GameWorld::checkPointCollision(const Point& point, GameObject* excludeObj) {
    // Проверяем границы поля
    if (point.x < 0 || point.y < 0 || point.x >= fieldWidth || point.y >= fieldHeight) {
        return true;
    }
    
    // Проверяем столкновения с объектами
    for (const auto& obj : objects) {
        if (!obj || obj->isDestroyed() || obj.get() == excludeObj) continue;
        
        Point objPos = obj->getPosition();
        Point objBounds = obj->getBounds();
        
        // Проверяем попадание в объект
        if (point.x >= objPos.x && point.x < objPos.x + objBounds.x &&
            point.y >= objPos.y && point.y < objPos.y + objBounds.y) {
            
            // Если это непроходимое препятствие - столкновение
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
            if (obstacle && !obstacle->isPassable()) {
                return true;
            }
            
            // Если это другой танк - столкновение
            Tank* otherTank = dynamic_cast<Tank*>(obj.get());
            if (otherTank) {
                return true;
            }
        }
    }
    
    return false;
}

void GameWorld::spawnBonus() {
    // Генерируем случайную позицию для бонуса
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(1, fieldWidth - 2);
    std::uniform_int_distribution<> yDist(1, fieldHeight - 2);
    
    // Выбираем случайный тип бонуса
    std::uniform_int_distribution<> typeDist(0, 3);
    BonusType type = static_cast<BonusType>(typeDist(gen));
    
    // Пытаемся найти валидную позицию для бонуса
    Point bonusPos;
    bool validPositionFound = false;
    int maxAttempts = 50; // Максимальное количество попыток
    
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        bonusPos = Point(xDist(gen), yDist(gen));
        
        if (isValidBonusPosition(bonusPos)) {
            validPositionFound = true;
            break;
        }
    }
    
    // Если не нашли валидную позицию после всех попыток, пропускаем спавн
    if (!validPositionFound) {
        return;
    }
    
    // Создаем бонус
    auto bonus = std::make_unique<Bonus>(bonusPos, type);
    bonuses.push_back(std::move(bonus));
}

bool GameWorld::isValidBonusPosition(const Point& pos) const {
    // Проверяем границы поля
    if (pos.x < 1 || pos.x >= fieldWidth - 1 || 
        pos.y < 1 || pos.y >= fieldHeight - 1) {
        return false;
    }
    
    // Проверяем, что позиция доступна для игрока (не на непроходимом препятствии)
    for (const auto& obj : objects) {
        if (!obj || obj->isDestroyed()) continue;
        
        Point objPos = obj->getPosition();
        Point objBounds = obj->getBounds();
        
        // Проверяем, находится ли позиция бонуса внутри объекта
        if (pos.x >= objPos.x && pos.x < objPos.x + objBounds.x &&
            pos.y >= objPos.y && pos.y < objPos.y + objBounds.y) {
            
            // Если это непроходимое препятствие - позиция невалидна
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
            if (obstacle && !obstacle->isPassable()) {
                return false;
            }
            
            // Если это другой бонус - позиция невалидна (избегаем наложения)
            Bonus* existingBonus = dynamic_cast<Bonus*>(obj.get());
            if (existingBonus && existingBonus->isActive()) {
                return false;
            }
            
            // Если это танк - позиция невалидна
            Tank* tank = dynamic_cast<Tank*>(obj.get());
            if (tank) {
                return false;
            }
        }
    }
    
    // Проверяем, что вокруг позиции есть достаточно места для подбора
    std::vector<Point> adjacentPositions = {
        Point(pos.x - 1, pos.y),    // слева
        Point(pos.x + 1, pos.y),    // справа
        Point(pos.x, pos.y - 1),    // сверху
        Point(pos.x, pos.y + 1)     // снизу
    };
    
    int accessiblePositions = 0;
    for (const auto& adjacentPos : adjacentPositions) {
        if (isPositionAccessible(adjacentPos)) {
            accessiblePositions++;
        }
    }
    
    return accessiblePositions >= 1;
}

bool GameWorld::isPositionAccessible(const Point& pos) const {
    // Проверяем границы поля
    if (pos.x < 0 || pos.x >= fieldWidth || pos.y < 0 || pos.y >= fieldHeight) {
        return false;
    }
    
    // Проверяем, что позиция проходима для танка
    for (const auto& obj : objects) {
        if (!obj || obj->isDestroyed()) continue;
        
        Point objPos = obj->getPosition();
        Point objBounds = obj->getBounds();
        
        // Проверяем, находится ли позиция внутри объекта
        if (pos.x >= objPos.x && pos.x < objPos.x + objBounds.x &&
            pos.y >= objPos.y && pos.y < objPos.y + objBounds.y) {
            
            // Если это непроходимое препятствие - позиция недоступна
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
            if (obstacle && !obstacle->isPassable()) {
                return false;
            }
            
            // Если это танк - позиция недоступна
            Tank* tank = dynamic_cast<Tank*>(obj.get());
            if (tank) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<GameObject*> GameWorld::getObjectsInRadius(Point center, int radius) const {
    std::vector<GameObject*> result;
    
    for (const auto& obj : objects) {
        if (!obj || obj->isDestroyed()) continue;
        
        Point objPos = obj->getPosition();
        int distance = abs(objPos.x - center.x) + abs(objPos.y - center.y);
        
        if (distance <= radius) { result.push_back(obj.get()); }
    }
    
    return result;
}

void GameWorld::setState(GameState newState) { 
    state = newState; 
}

GameState GameWorld::getState() const { 
    return state; 
}

int GameWorld::getWidth() const { 
    return fieldWidth; 
}

int GameWorld::getHeight() const { 
    return fieldHeight; 
}

PlayerTank* GameWorld::getPlayer() const { 
    return player; 
}

void GameWorld::setPlayer(PlayerTank* newPlayer) {
    player = newPlayer;
}

const std::vector<std::unique_ptr<GameObject>>& GameWorld::getObjects() const { 
    return objects; 
}

const std::vector<std::unique_ptr<Projectile>>& GameWorld::getProjectiles() const { 
    return projectiles; 
}

const std::vector<std::unique_ptr<Bonus>>& GameWorld::getBonuses() const { 
    return bonuses; 
}

int GameWorld::getCurrentLevel() const { 
    return currentLevel; 
}

void GameWorld::addObject(std::unique_ptr<GameObject> obj) {
    objects.push_back(std::move(obj));
    
    // Обновляем счетчик врагов
    if (dynamic_cast<EnemyTank*>(objects.back().get())) { enemyCount++; }
}

void GameWorld::addProjectile(std::unique_ptr<Projectile> proj) { 
    projectiles.push_back(std::move(proj)); 
}

void GameWorld::addBonus(std::unique_ptr<Bonus> bonus) { 
    bonuses.push_back(std::move(bonus)); 
}

void GameWorld::playerFire() {
    if (state != GameState::PLAYING) return;
    
    Projectile* projectile = player->fire();
    if (projectile) { projectiles.emplace_back(projectile); }
}

void GameWorld::playerMove(Direction dir) {
    if (state != GameState::PLAYING) return;
    
    // Сохраняем начальную позицию
    Point startPos = player->getPosition();
    Point bounds = player->getBounds();
    
    player->rotate(dir);
    
    // Вычисляем фактическую скорость с учетом бонусов
    int actualSpeed = player->getSpeed();
    if (player->getSpeedBoostDuration() > 0) {
        actualSpeed += 0; // Учитываем бонус скорости
    }
    if (actualSpeed > 2) actualSpeed = 2;
    
    // Пытаемся двигаться пошагово, проверяя каждую клетку
    Point currentPos = startPos;
    
    for (int step = 0; step < actualSpeed; step++) {
        Point testPos = currentPos;
        
        // Вычисляем позицию для следующего шага
        switch (dir) {
            case Direction::UP:
                testPos.y -= 1;
                break;
            case Direction::DOWN:
                testPos.y += 1;
                break;
            case Direction::LEFT:
                testPos.x -= 1;
                break;
            case Direction::RIGHT:
                testPos.x += 1;
                break;
        }
        
        // Проверяем валидность новой позиции
        if (isValidPosition(testPos, bounds, player)) {
            currentPos = testPos;
            
            // НЕМЕДЛЕННАЯ проверка бонусов на новой позиции
            Point checkPos = currentPos;
            for (auto& bonus : bonuses) {
                if (!bonus || !bonus->isActive()) continue;
                
                Point bonusPos = bonus->getPosition();
                
                // Точное совпадение позиций
                if (checkPos.x == bonusPos.x && checkPos.y == bonusPos.y) {
                    bonus->applyEffect(player);
                    
                    if (bonus->getType() == BonusType::SPEED_BOOST) {
                        applySlowToEnemies(20);
                    }

                    bonus->deactivate();
                    player->addScore(50);
                    break; // Один бонус за шаг
                }
            }
        } else {
            // Если не можем двигаться дальше, останавливаемся
            break;
        }
    }
    
    // Устанавливаем финальную позицию только если смогли сдвинуться
    if (currentPos != startPos) {
        player->setPosition(currentPos);
    }
    
    // Дополнительная проверка границ
    Point finalPos = player->getPosition();
    if (finalPos.x < 0) player->setPosition(Point(0, finalPos.y));
    if (finalPos.y < 0) player->setPosition(Point(finalPos.x, 0));
    if (finalPos.x + bounds.x >= fieldWidth) 
        player->setPosition(Point(fieldWidth - bounds.x, finalPos.y));
    if (finalPos.y + bounds.y >= fieldHeight) 
        player->setPosition(Point(finalPos.x, fieldHeight - bounds.y));
    
    // Финальная проверка бонусов (на случай если что-то пропустили)
    Point finalCheckPos = player->getPosition();
    for (auto& bonus : bonuses) {
        if (!bonus || !bonus->isActive()) continue;
        
        Point bonusPos = bonus->getPosition();
        
        if (finalCheckPos.x == bonusPos.x && finalCheckPos.y == bonusPos.y) {
            bonus->applyEffect(player);
            bonus->deactivate();
            player->addScore(50);
        }
    }
}

// Приватные методы

GameWorld::DifficultyParams GameWorld::adjustDifficulty(int level) {
    DifficultyParams params;
    
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

bool GameWorld::isValidPosition(const Point& pos, const Point& bounds, const GameObject* excludeObj) const {
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
        
        // Проверяем пересечение с учетом размеров объектов
        bool collisionX = pos.x < objPos.x + objBounds.x && pos.x + bounds.x > objPos.x;
        bool collisionY = pos.y < objPos.y + objBounds.y && pos.y + bounds.y > objPos.y;
        
        if (collisionX && collisionY) {
            // Если это непроходимое препятствие - позиция невалидна
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
            if (obstacle && !obstacle->isPassable()) {
                return false;
            }
            
            // Если это другой танк - позиция невалидна
            Tank* otherTank = dynamic_cast<Tank*>(obj.get());
            if (otherTank) {
                return false;
            }
        }
    }
    
    return true;
}

void GameWorld::createEnemies(int level) {
    enemyCount = 0;
    maxEnemies = 3 + level;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> xDist(2, fieldWidth - 4);
    std::uniform_int_distribution<> yDist(2, fieldHeight / 2); // Враги в верхней части
    std::uniform_int_distribution<> behaviorDist(0, 2);
    
    // Распределение типов танков в зависимости от уровня
    std::uniform_int_distribution<> typeDist(0, 100);
    
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
        
        // Определяем тип танка на основе вероятностей
        EnemyTankType tankType = EnemyTankType::BASIC;
        int typeRoll = typeDist(gen);
        
        if (level >= 2) {
            if (typeRoll < 50) { // 50% - обычные танки
                tankType = EnemyTankType::BASIC;
            } else if (typeRoll < 70) { // 20% - быстрые танки
                tankType = EnemyTankType::FAST;
            } else if (typeRoll < 85) { // 15% - танки с уроном
                tankType = EnemyTankType::DAMAGE;
            } else { // 15% - бронированные танки
                tankType = EnemyTankType::ARMORED;
            }
        } else {
            // На первом уровне только обычные танки
            tankType = EnemyTankType::BASIC;
        }
        
        // На высоких уровнях увеличиваем шанс появления специальных танков
        if (level >= 4) {
            typeRoll = typeDist(gen); // Перебрасываем кубик
            if (typeRoll < 30) { // 30% - обычные
                tankType = EnemyTankType::BASIC;
            } else if (typeRoll < 55) { // 25% - быстрые
                tankType = EnemyTankType::FAST;
            } else if (typeRoll < 75) { // 20% - с уроном
                tankType = EnemyTankType::DAMAGE;
            } else { // 25% - бронированные
                tankType = EnemyTankType::ARMORED;
            }
        }
        
        auto enemy = std::make_unique<EnemyTank>(pos, behavior, difficulty, tankType);
        objects.push_back(std::move(enemy));
        enemyCount++;
    }
}

void GameWorld::updateEnemyAI() {
    Point playerPos = player->getPosition();
    
    for (auto& obj : objects) {
        EnemyTank* enemy = dynamic_cast<EnemyTank*>(obj.get());
        if (enemy && !enemy->isDestroyed()) {
            // Обновляем позицию игрока для ИИ врага
            enemy->setPlayerPosition(playerPos);
            
            // ДОБАВЛЯЕМ: Обновляем врага с проверкой столкновений
            updateEnemyMovement(enemy);
        }
    }
}

void GameWorld::updateEnemyMovement(EnemyTank* enemy) {
    if (!enemy) return;
    
    // Сохраняем начальную позицию
    Point startPos = enemy->getPosition();
    Point bounds = enemy->getBounds();
    
    // Получаем направление движения врага
    Direction dir = enemy->getDirection();
    
    // Вычисляем новую позицию с учетом скорости
    int speed = enemy->getActualSpeed();
    if (speed <= 0) return;

    // Проверяем каждый шаг движения для точного определения столкновений
    Point currentPos = startPos;
    bool canMove = true;
    
    for (int step = 0; step < speed && canMove; step++) {
        Point nextPos = currentPos;
        
        switch (dir) {
            case Direction::UP: nextPos.y -= 1; break;
            case Direction::DOWN: nextPos.y += 1; break;
            case Direction::LEFT: nextPos.x -= 1; break;
            case Direction::RIGHT: nextPos.x += 1; break;
        }
        
        // Проверяем валидность позиции для этого шага
        if (isValidPosition(nextPos, bounds, enemy)) {
            currentPos = nextPos;
        } else {
            canMove = false;
        }
    }
    
    if (currentPos != startPos) {
        // Если можем двигаться - устанавливаем новую позицию
        enemy->setPosition(currentPos);
    } else {
        // Если не можем двигаться - обрабатываем столкновение
        handleEnemyCollision(enemy, dir);
    }
}

void GameWorld::handleEnemyCollision(EnemyTank* enemy, Direction moveDir) {
    if (!enemy) return;
    
    // При столкновении враг меняет направление
    std::vector<Direction> possibleDirections = {
        Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT
    };
    
    // Убираем текущее направление и противоположное (чтобы не ходить туда-сюда)
    Direction oppositeDir;
    switch (moveDir) {
        case Direction::UP: oppositeDir = Direction::DOWN; break;
        case Direction::DOWN: oppositeDir = Direction::UP; break;
        case Direction::LEFT: oppositeDir = Direction::RIGHT; break;
        case Direction::RIGHT: oppositeDir = Direction::LEFT; break;
    }
    
    possibleDirections.erase(
        std::remove(possibleDirections.begin(), possibleDirections.end(), moveDir),
        possibleDirections.end()
    );
    possibleDirections.erase(
        std::remove(possibleDirections.begin(), possibleDirections.end(), oppositeDir),
        possibleDirections.end()
    );
    
    // Пробуем оставшиеся направления пока не найдем валидное
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(possibleDirections.begin(), possibleDirections.end(), gen);
    
    Point bounds = enemy->getBounds();
    Point currentPos = enemy->getPosition();
    
    for (Direction newDir : possibleDirections) {
        Point testPos = currentPos;
        
        // Проверяем только один шаг в новом направлении
        switch (newDir) {
            case Direction::UP: testPos.y -= 1; break;
            case Direction::DOWN: testPos.y += 1; break;
            case Direction::LEFT: testPos.x -= 1; break;
            case Direction::RIGHT: testPos.x += 1; break;
        }
        
        if (isValidPosition(testPos, bounds, enemy)) {
            enemy->rotate(newDir);
            break;
        }
    }
    
    // Если не нашли валидное направление, враг остается на месте в текущем направлении
}

void GameWorld::cleanupDestroyedObjects() {
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
        
    // Удаляем завершенные взрывы (те, у которых lifetime <= 0)
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
        [](const std::unique_ptr<Explosion>& explosion) {
            return explosion->isDestroyed();
        }), explosions.end());
}

void GameWorld::addExplosion(std::unique_ptr<Explosion> explosion) {
    explosions.push_back(std::move(explosion));
}

const std::vector<std::unique_ptr<Explosion>>& GameWorld::getExplosions() const {
    return explosions;
}

void GameWorld::checkGameConditions() {
    if (!player) return;
    
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
        return;
    }
    
    // Проверяем завершение уровня только если игрок жив
    if (enemyCount == 0 && state == GameState::PLAYING && currentLevel > 0) {
        if (player) {
            player->addScore(500);
        }
        state = GameState::LEVEL_COMPLETE;
    }
}


void GameWorld::createLevelObstacles(int level) {
    // Очищаем существующие препятствия (кроме границ)
    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](const std::unique_ptr<GameObject>& obj) {
            Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
            return obstacle != nullptr;
        }), objects.end());

    // Создаем границы поля
    for (int x = 0; x < fieldWidth; x++) {
        objects.emplace_back(new Obstacle(Point(x, 0), ObstacleType::STEEL));
        objects.emplace_back(new Obstacle(Point(x, fieldHeight - 1), ObstacleType::STEEL));
    }
    
    for (int y = 1; y < fieldHeight - 1; y++) {
        objects.emplace_back(new Obstacle(Point(0, y), ObstacleType::STEEL));
        objects.emplace_back(new Obstacle(Point(fieldWidth - 1, y), ObstacleType::STEEL));
    }

    // Инициализация генератора случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    std::uniform_int_distribution<> posDist(2, fieldWidth - 3);
    
    // 1. ПРОГРЕССИЯ СЛОЖНОСТИ - настраиваем параметры в зависимости от уровня
    DifficultyParams difficultyParams = adjustDifficulty(level);
    
    // Целевые количества препятствий по типам с учетом прогрессии
    struct ObstacleTarget {
        ObstacleType type;
        int targetCount;
        int currentCount;
    };
    
    std::vector<ObstacleTarget> targets = {
        {ObstacleType::BRICK, difficultyParams.brickCount, 0},
        {ObstacleType::STEEL, difficultyParams.steelCount, 0},
        {ObstacleType::WATER, difficultyParams.waterCount, 0},
        {ObstacleType::FOREST, difficultyParams.forestCount, 0}
    };
    
    // Функция проверки доступности позиции
    auto isValidPosition = [this](const Point& pos, int minDistFromPlayer = 3, int minDistFromBorder = 2) -> bool {
        // Проверяем расстояние до границ
        if (pos.x < minDistFromBorder || pos.x >= fieldWidth - minDistFromBorder ||
            pos.y < minDistFromBorder || pos.y >= fieldHeight - minDistFromBorder) {
            return false;
        }
        
        // Проверяем расстояние до игрока
        if (player) {
            Point playerPos = player->getPosition();
            int distX = abs(pos.x - playerPos.x);
            int distY = abs(pos.y - playerPos.y);
            if (distX < minDistFromPlayer && distY < minDistFromPlayer) {
                return false;
            }
        }
        
        // Проверяем пересечение с существующими объектами
        for (const auto& obj : objects) {
            if (!obj) continue;
            
            Point objPos = obj->getPosition();
            Point objBounds = obj->getBounds();
            
            if (pos.x >= objPos.x && pos.x < objPos.x + objBounds.x &&
                pos.y >= objPos.y && pos.y < objPos.y + objBounds.y) {
                return false;
            }
        }
        
        return true;
    };
    
    // 2. УЛУЧШЕННЫЙ АЛГОРИТМ ОРГАНИЧЕСКИХ ФОРМ
    
    // Функция создания органичного кластера с алгоритмом роста
    auto createOrganicCluster = [&](Point center, int maxRadius, int targetCount, ObstacleType type) {
        auto targetIt = std::find_if(targets.begin(), targets.end(),
            [type](const ObstacleTarget& t) { return t.type == type; });
        
        if (targetIt == targets.end() || targetIt->currentCount >= targetIt->targetCount) return;
        
        std::vector<Point> clusterPoints;
        std::vector<Point> activePoints;
        
        // Начинаем с центральной точки
        if (isValidPosition(center, 2)) {
            clusterPoints.push_back(center);
            activePoints.push_back(center);
            objects.emplace_back(new Obstacle(center, type));
            targetIt->currentCount++;
        }
        
        // Алгоритм роста для органичной формы
        int maxIterations = targetCount * 5; // Ограничиваем количество итераций
        int iterations = 0;
        
        while (!activePoints.empty() && clusterPoints.size() < static_cast<size_t>(targetCount) && iterations < maxIterations) {
            iterations++;
            
            // Выбираем случайную активную точку
            int randomIndex = rand() % activePoints.size();
            Point basePoint = activePoints[randomIndex];
            
            // Направления для роста (включая диагонали для более органичной формы)
            std::vector<Point> directions = {
                Point(1,0), Point(-1,0), Point(0,1), Point(0,-1),
                Point(1,1), Point(1,-1), Point(-1,1), Point(-1,-1)
            };
            
            bool addedNewPoint = false;
            
            for (const auto& dir : directions) {
                Point newPoint(basePoint.x + dir.x, basePoint.y + dir.y);
                
                // Проверяем расстояние от центра
                double distance = sqrt(pow(newPoint.x - center.x, 2) + pow(newPoint.y - center.y, 2));
                if (distance > maxRadius) continue;
                
                // Проверяем валидность и отсутствие дубликатов
                if (isValidPosition(newPoint, 2) && 
                    std::find(clusterPoints.begin(), clusterPoints.end(), newPoint) == clusterPoints.end()) {
                    
                    // Шанс добавления уменьшается с расстоянием от центра
                    double distanceFactor = 1.0 - (distance / maxRadius);
                    double addChance = 0.7 * distanceFactor; // Базовый шанс 70%, уменьшается с расстоянием
                    
                    if (dist(gen) < addChance) {
                        clusterPoints.push_back(newPoint);
                        activePoints.push_back(newPoint);
                        objects.emplace_back(new Obstacle(newPoint, type));
                        targetIt->currentCount++;
                        addedNewPoint = true;
                        
                        if (clusterPoints.size() >= static_cast<size_t>(targetCount)) break;
                    }
                }
            }
            
            // Если из этой точки не выросло ничего нового, удаляем ее из активных
            if (!addedNewPoint) {
                activePoints.erase(activePoints.begin() + randomIndex);
            }
            
            if (clusterPoints.size() >= static_cast<size_t>(targetCount)) break;
        }
    };
    
    // Функция создания органичных стен
    auto createOrganicWall = [&](Point start, Point end, ObstacleType type, double thickness = 1.0) {
        auto targetIt = std::find_if(targets.begin(), targets.end(),
            [type](const ObstacleTarget& t) { return t.type == type; });
        
        if (targetIt == targets.end()) return;
        
        // Основное направление стены
        bool horizontal = abs(end.y - start.y) < abs(end.x - start.x);
        
        if (horizontal) {
            int minX = std::min(start.x, end.x);
            int maxX = std::max(start.x, end.x);
            int y = start.y;
            
            for (int x = minX; x <= maxX; x++) {
                // Добавляем вариативность по толщине и положению
                for (int dy = -static_cast<int>(thickness); dy <= static_cast<int>(thickness); dy++) {
                    Point pos(x, y + dy);
                    
                    // Шанс добавления блока для неровной поверхности
                    if (isValidPosition(pos, 2) && dist(gen) < 0.8) {
                        if (targetIt->currentCount < targetIt->targetCount) {
                            objects.emplace_back(new Obstacle(pos, type));
                            targetIt->currentCount++;
                        }
                    }
                }
            }
        } else {
            int minY = std::min(start.y, end.y);
            int maxY = std::max(start.y, end.y);
            int x = start.x;
            
            for (int y = minY; y <= maxY; y++) {
                for (int dx = -static_cast<int>(thickness); dx <= static_cast<int>(thickness); dx++) {
                    Point pos(x + dx, y);
                    
                    if (isValidPosition(pos, 2) && dist(gen) < 0.8) {
                        if (targetIt->currentCount < targetIt->targetCount) {
                            objects.emplace_back(new Obstacle(pos, type));
                            targetIt->currentCount++;
                        }
                    }
                }
            }
        }
    };
    
    // 3. СОЗДАЕМ ОСНОВНЫЕ СТРУКТУРЫ УРОВНЯ С УЧЕТОМ ПРОГРЕССИИ
    
    // Создаем органичные кластеры для каждого типа препятствий
    if (difficultyParams.organicClusters) {
        // Кирпичные кластеры
        for (int i = 0; i < difficultyParams.brickClusters; i++) {
            Point center(posDist(gen), posDist(gen));
            int clusterSize = 8 + level; // Размер кластера растет с уровнем
            createOrganicCluster(center, 4 + level/2, clusterSize, ObstacleType::BRICK);
        }
        
        // Стальные укрепления (появляются на высоких уровнях)
        if (level >= 3) {
            for (int i = 0; i < difficultyParams.steelClusters; i++) {
                Point center(posDist(gen), posDist(gen));
                createOrganicCluster(center, 3, 5 + level/2, ObstacleType::STEEL);
            }
        }
        
        // Лесные массивы
        for (int i = 0; i < difficultyParams.forestClusters; i++) {
            Point center(posDist(gen), posDist(gen));
            createOrganicCluster(center, 5, 10 + level, ObstacleType::FOREST);
        }
        
        // Водоемы
        for (int i = 0; i < difficultyParams.waterClusters; i++) {
            Point center(posDist(gen), posDist(gen));
            createOrganicCluster(center, 4, 8 + level, ObstacleType::WATER);
        }
    }
    
    // 4. СОЗДАЕМ СТРАТЕГИЧЕСКИЕ СТРУКТУРЫ
    
    // Защитные стены вокруг стартовой позиции игрока
    if (level >= 2) {
        Point playerStart(fieldWidth / 2, fieldHeight - 3);
        createOrganicWall(Point(playerStart.x - 4, playerStart.y - 2), 
                         Point(playerStart.x + 4, playerStart.y - 2), 
                         ObstacleType::BRICK, 1.2);
    }
    
    // Укрепленные позиции врагов (появляются на высоких уровнях)
    if (level >= 4) {
        std::vector<Point> enemyFortressCenters = {
            Point(fieldWidth / 4, fieldHeight / 4),
            Point(3 * fieldWidth / 4, fieldHeight / 4),
            Point(fieldWidth / 2, fieldHeight / 3)
        };
        
        for (const auto& center : enemyFortressCenters) {
            // Внешнее кольцо из кирпича
            createOrganicWall(Point(center.x - 3, center.y - 3), 
                             Point(center.x + 3, center.y - 3), ObstacleType::BRICK);
            createOrganicWall(Point(center.x - 3, center.y + 3), 
                             Point(center.x + 3, center.y + 3), ObstacleType::BRICK);
            createOrganicWall(Point(center.x - 3, center.y - 3), 
                             Point(center.x - 3, center.y + 3), ObstacleType::BRICK);
            createOrganicWall(Point(center.x + 3, center.y - 3), 
                             Point(center.x + 3, center.y + 3), ObstacleType::BRICK);
            
            // Внутреннее укрепление из стали
            if (level >= 6) {
                createOrganicCluster(center, 2, 4, ObstacleType::STEEL);
            }
        }
    }
    
    // 5. ДОБАВЛЯЕМ СЛУЧАЙНЫЕ ЭЛЕМЕНТЫ ДЛЯ ЕСТЕСТВЕННОСТИ
    
    // Случайные одиночные препятствия для заполнения пустот
    for (auto& target : targets) {
        int remaining = target.targetCount - target.currentCount;
        if (remaining > 0) {
            for (int i = 0; i < remaining * 3; i++) {
                Point pos(posDist(gen), posDist(gen));
                
                if (isValidPosition(pos, 2) && dist(gen) < 0.3) {
                    objects.emplace_back(new Obstacle(pos, target.type));
                    target.currentCount++;
                    
                    if (target.currentCount >= target.targetCount) break;
                }
            }
        }
    }
    
    // 6. ГАРАНТИРУЕМ ПРОХОДИМОСТЬ
    
    // Создаем основную сеть путей (более органичную на высоких уровнях)
    std::vector<Point> pathNodes;
    
    if (level <= 3) {
        // Простые прямые пути для начальных уровней
        for (int x = 3; x < fieldWidth - 3; x += 6) {
            for (int y = 3; y < fieldHeight - 3; y += 2) {
                pathNodes.emplace_back(x, y);
            }
        }
    } else {
        // Более извилистые органичные пути для сложных уровней
        for (int x = 3; x < fieldWidth - 3; x += 4 + level/2) {
            for (int y = 3; y < fieldHeight - 3; y += 3) {
                // Добавляем случайное смещение для органичности
                int offsetX = (rand() % 3) - 1;
                int offsetY = (rand() % 3) - 1;
                pathNodes.emplace_back(x + offsetX, y + offsetY);
            }
        }
    }
    
    // Очищаем области вокруг узлов пути
    for (const auto& node : pathNodes) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                Point clearPoint(node.x + dx, node.y + dy);
                objects.erase(std::remove_if(objects.begin(), objects.end(),
                    [&clearPoint](const std::unique_ptr<GameObject>& obj) {
                        Obstacle* obstacle = dynamic_cast<Obstacle*>(obj.get());
                        if (!obstacle) return false;
                        Point objPos = obstacle->getPosition();
                        return objPos.x == clearPoint.x && objPos.y == clearPoint.y &&
                               obstacle->getType() != ObstacleType::STEEL;
                    }), objects.end());
            }
        }
    }
    
    // 7. ФИНАЛЬНАЯ КОРРЕКТИРОВКА С УЧЕТОМ УРОВНЯ СЛОЖНОСТИ
    
    // На высоких уровнях добавляем дополнительные препятствия вокруг игрока
    if (level >= 5) {
        Point playerPos = player->getPosition();
        std::vector<Point> defensivePositions = {
            Point(playerPos.x - 2, playerPos.y - 1), Point(playerPos.x + 2, playerPos.y - 1),
            Point(playerPos.x - 1, playerPos.y - 2), Point(playerPos.x + 1, playerPos.y - 2)
        };
        
        for (const auto& pos : defensivePositions) {
            if (isValidPosition(pos, 1) && dist(gen) < 0.6) {
                objects.emplace_back(new Obstacle(pos, ObstacleType::BRICK));
            }
        }
    }
    
    // Заполняем оставшиеся квоты случайным образом
    for (auto& target : targets) {
        while (target.currentCount < target.targetCount) {
            Point pos(posDist(gen), posDist(gen));
            
            if (isValidPosition(pos, 2) && dist(gen) < 0.5) {
                objects.emplace_back(new Obstacle(pos, target.type));
                target.currentCount++;
            }
        }
    }
}

int GameWorld::getEnemyCount() const { 
    return enemyCount; 
}

void GameWorld::setCurrentLevel(int level) {
    currentLevel = level;
}

void GameWorld::triggerDamageFlash() {
    damageFlashCounter = 2; // Эффект будет активен 2 хода (можно настроить)
}

bool GameWorld::isDamageFlashActive() const {
    return damageFlashCounter > 0;
}
