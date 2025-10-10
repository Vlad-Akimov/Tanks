#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <cmath>
#include <map>
#include "GameWorld.cpp"

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
        
        while (!activePoints.empty() && clusterPoints.size() < targetCount && iterations < maxIterations) {
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
                        
                        if (clusterPoints.size() >= targetCount) break;
                    }
                }
            }
            
            // Если из этой точки не выросло ничего нового, удаляем ее из активных
            if (!addedNewPoint) {
                activePoints.erase(activePoints.begin() + randomIndex);
            }
            
            if (clusterPoints.size() >= targetCount) break;
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