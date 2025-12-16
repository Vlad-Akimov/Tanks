/**
 * @file MapManager.cpp
 * @author Vld251
 * @brief Implementation for loading, creating, and managing game maps.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "MapManager.h"
#include "../model/PlayerTank.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h> 

#ifdef _WIN32
    #include <direct.h>
    #define mkdir _mkdir
#endif

MapManager::MapManager(const std::string& directory) 
    : mapsDirectory(directory) {
    // Сначала создаем карты по умолчанию
    createDefaultMaps();
    // Затем загружаем (это перезапишет существующие файлы)
    loadMaps();
}

bool MapManager::loadMaps() {
    maps.clear();
    
    try {
        // Создаем директорию если не существует
        mkdir(mapsDirectory.c_str(), 0755);
        
        // Проверяем существование директории
        DIR* dir = opendir(mapsDirectory.c_str());
        if (!dir) {
            std::cerr << "Directory does not exist: " << mapsDirectory << std::endl;
            createDefaultMaps();
            return false;
        }
        
        // Ищем все .map файлы в директории
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;
            
            // Проверяем расширение .map
            if (filename.length() > 4 && 
                filename.substr(filename.length() - 4) == ".map") {
                
                std::string fullPath = mapsDirectory + "/" + filename;
                std::cout << "Found map file: " << fullPath << std::endl;
                
                MapInfo mapInfo;
                mapInfo.filename = filename;
                
                // Читаем информацию о карте из файла
                std::ifstream file(fullPath);
                if (file.is_open()) {
                    std::string line;
                    
                    // Читаем displayName (первая строка)
                    if (std::getline(file, line)) {
                        mapInfo.displayName = line;
                        std::cout << "DisplayName: " << mapInfo.displayName << std::endl;
                    } else {
                        std::cerr << "Error: map file is empty or corrupted: " << mapInfo.filename << std::endl;
                        continue;
                    }
                    
                    // Читаем description (вторая строка)
                    if (std::getline(file, line)) {
                        mapInfo.description = line;
                        std::cout << "Description: " << mapInfo.description << std::endl;
                    }
                    
                    // Читаем размеры (третья строка)
                    if (std::getline(file, line)) {
                        std::istringstream iss(line);
                        if (!(iss >> mapInfo.width >> mapInfo.height)) {
                            std::cerr << "Error reading map dimensions from file: " << mapInfo.filename << std::endl;
                            continue;
                        }
                        std::cout << "Size: " << mapInfo.width << "x" << mapInfo.height << std::endl;
                    }
                    
                    // Читаем layout (остальные строки)
                    mapInfo.layout.clear();
                    int linesRead = 0;
                    while (std::getline(file, line)) {
                        // Пропускаем пустые строки
                        if (line.empty()) continue;
                        
                        if (line.length() == static_cast<size_t>(mapInfo.width)) {
                            std::vector<char> row(line.begin(), line.end());
                            mapInfo.layout.push_back(row);
                            linesRead++;
                        } else if (!line.empty()) {
                                std::cout << "Skipped line with incorrect length: " << line.length() << " instead of " << mapInfo.width << " in file " << mapInfo.filename << std::endl;
                        }
                    }
                    
                    file.close();
                    
                    // Проверяем корректность данных
                    if (mapInfo.layout.size() == static_cast<size_t>(mapInfo.height)) {
                        maps.push_back(mapInfo);
                        std::cout << "Successfully loaded map: " << mapInfo.displayName << " (" << linesRead << " lines)" << std::endl;
                    } else {
                        std::cerr << "Incorrect number of lines in map " << mapInfo.filename << ": " << mapInfo.layout.size() << " instead of " << mapInfo.height << std::endl;
                    }
                } else {
                    std::cerr << "Failed to open file: " << fullPath << std::endl;
                }
            }
        }
        
        closedir(dir);
        
        // Если карт нет, создаем несколько примеров
        if (maps.empty()) {
            std::cout << "No maps found, creating default maps..." << std::endl;
            createDefaultMaps();
        } else {
            std::cout << "Total maps loaded: " << maps.size() << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading maps: " << e.what() << std::endl;
        createDefaultMaps();
        return false;
    }
}

void MapManager::createDefaultMaps() {
    std::cout << "Creating default maps..." << std::endl;
    
    // Создаем директорию, если она не существует
    mkdir(mapsDirectory.c_str(), 0755);
    
    // Карта 1: Классическая
    MapInfo map1;
    map1.filename = "classic.map";
    map1.displayName = "Classic Arena";
    map1.description = "A classic map for beginners with a balance of cover and open spaces";
    map1.width = 40;
    map1.height = 20;
    
    // Создаем layout для классической карты
    map1.layout = std::vector<std::vector<char>>(map1.height, 
                std::vector<char>(map1.width, ' '));
    
    // Границы
    for (int x = 0; x < map1.width; x++) {
        map1.layout[0][x] = 'X';
        map1.layout[map1.height-1][x] = 'X';
    }
    for (int y = 0; y < map1.height; y++) {
        map1.layout[y][0] = 'X';
        map1.layout[y][map1.width-1] = 'X';
    }
    
    // Препятствия
    for (int i = 5; i < 15; i++) {
        map1.layout[5][i] = '#';
        map1.layout[15][i] = '#';
    }
    
    // Вода
    for (int i = 25; i < 35; i++) {
        map1.layout[8][i] = '~';
        map1.layout[9][i] = '~';
    }
    
    // Лес
    for (int i = 10; i < 20; i++) {
        map1.layout[12][i] = '*';
        map1.layout[13][i] = '*';
    }
    
    // Враги
    map1.layout[3][10] = 'E';
    map1.layout[3][30] = 'E';
    
    // Сохраняем в файл
    saveMapToFile(map1);
    maps.push_back(map1);
    
    // Карта 2: Крепость
    MapInfo map2;
    map2.filename = "fortress.map";
    map2.displayName = "The Fortress";
    map2.description = "A fortified castle with complex defenses";
    map2.width = 40;
    map2.height = 20;
    
    map2.layout = std::vector<std::vector<char>>(map2.height, 
                std::vector<char>(map2.width, ' '));
    
    // Границы
    for (int x = 0; x < map2.width; x++) {
        map2.layout[0][x] = 'X';
        map2.layout[map2.height-1][x] = 'X';
    }
    for (int y = 0; y < map2.height; y++) {
        map2.layout[y][0] = 'X';
        map2.layout[y][map2.width-1] = 'X';
    }
    
    // Крепость
    for (int x = 5; x < 20; x++) {
        map2.layout[5][x] = '#';
        map2.layout[12][x] = '#';
    }
    for (int y = 5; y <= 12; y++) {
        map2.layout[y][5] = '#';
        map2.layout[y][19] = '#';
    }
    
    // Враги в крепости
    map2.layout[8][10] = 'E';
    map2.layout[8][15] = 'E';
    
    // Вода вокруг
    for (int x = 25; x < 35; x++) {
        map2.layout[8][x] = '~';
        map2.layout[9][x] = '~';
    }
    
    // Лес
    for (int x = 2; x < 5; x++) {
        for (int y = 15; y < 18; y++) {
            map2.layout[y][x] = '*';
        }
    }
    
    // Сохраняем в файл
    saveMapToFile(map2);
    maps.push_back(map2);
    
    // Карта 3: Поле боя
    MapInfo map3;
    map3.filename = "battlefield.map";
    map3.displayName = "Battlefield";
    map3.description = "Large field with cover and strategic positions";
    map3.width = 40;
    map3.height = 20;
    
    map3.layout = std::vector<std::vector<char>>(map3.height, 
                std::vector<char>(map3.width, ' '));
    
    // Границы
    for (int x = 0; x < map3.width; x++) {
        map3.layout[0][x] = 'X';
        map3.layout[map3.height-1][x] = 'X';
    }
    for (int y = 0; y < map3.height; y++) {
        map3.layout[y][0] = 'X';
        map3.layout[y][map3.width-1] = 'X';
    }
    
    // Бункеры
    for (int x = 5; x < 10; x++) {
        for (int y = 5; y < 8; y++) {
            map3.layout[y][x] = '#';
        }
    }
    
    for (int x = 30; x < 35; x++) {
        for (int y = 5; y < 8; y++) {
            map3.layout[y][x] = '#';
        }
    }
    
    // Река посередине
    for (int x = 15; x < 25; x++) {
        map3.layout[10][x] = '~';
        map3.layout[11][x] = '~';
    }
    
    // Враги
    map3.layout[3][8] = 'E';
    map3.layout[3][32] = 'E';
    map3.layout[16][8] = 'E';
    map3.layout[16][32] = 'E';
    
    // Сохраняем в файл
    saveMapToFile(map3);
    maps.push_back(map3);
    
    std::cout << "Created default maps: " << maps.size() << std::endl;
}

// Исправленный метод для сохранения карты в файл
void MapManager::saveMapToFile(const MapInfo& map) {
    // Правильное создание пути к файлу
    std::string filePath = mapsDirectory + "/" + map.filename;
    
    std::ofstream file(filePath);
    if (file.is_open()) {
        // Записываем метаданные
        file << map.displayName << "\n";
        file << map.description << "\n";
        file << map.width << " " << map.height << "\n";
        
        // Записываем layout
        for (const auto& row : map.layout) {
            for (char cell : row) {
                file << cell;
            }
            file << "\n";
        }
        
        file.close();
        std::cout << "Map saved: " << filePath << std::endl;
    } else {
        std::cerr << "Error saving map: " << filePath << std::endl;
    }
}

const std::vector<MapInfo>& MapManager::getMaps() const {
    return maps;
}

const MapInfo& MapManager::getMap(int index) const {
    if (index >= 0 && index < static_cast<int>(maps.size())) {
        return maps[index];
    }
    throw std::out_of_range("Invalid map index");
}

int MapManager::getMapCount() const {
    return maps.size();
}

bool MapManager::isValidMapIndex(int index) const {
    return index >= 0 && index < static_cast<int>(maps.size());
}

void MapManager::createWorldFromMap(GameWorld& world, const MapInfo& map) {
    int level = world.getCurrentLevel();
    
    // СОХРАНЯЕМ состояние игрока
    PlayerTank* oldPlayer = world.getPlayer();
    int savedScore = oldPlayer ? oldPlayer->getScore() : 0;
    int savedLives = oldPlayer ? oldPlayer->getLives() : 3;
    int savedHealth = oldPlayer ? oldPlayer->getHealth() : 3;
    
    // Получаем доступ к объектам мира для модификации
    auto& objects = const_cast<std::vector<std::unique_ptr<GameObject>>&>(world.getObjects());
    auto& projectiles = const_cast<std::vector<std::unique_ptr<Projectile>>&>(world.getProjectiles());
    auto& bonuses = const_cast<std::vector<std::unique_ptr<Bonus>>&>(world.getBonuses());
    
    // Очищаем ВСЕ объекты КРОМЕ игрока
    projectiles.clear();
    bonuses.clear();
    
    // Удаляем все объекты кроме игрока
    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [oldPlayer](const std::unique_ptr<GameObject>& obj) {
            return obj.get() != oldPlayer;
        }), objects.end());
    
    // ВОССТАНАВЛИВАЕМ состояние игрока
    if (oldPlayer) {
        oldPlayer->setPosition(Point(map.width / 2, map.height - 3));
        oldPlayer->setScore(savedScore);
        oldPlayer->setLives(savedLives);
        oldPlayer->setHealth(savedHealth);
    }
    
    int baseEnemyCount = countEnemiesInLayout(map.layout);
    int adjustedEnemyCount = calculateAdjustedEnemyCount(baseEnemyCount, level);
    int enemyDifficulty = calculateEnemyDifficulty(level);
    
    int enemyCount = 0;
    int obstacleCount = 0;
    std::vector<Point> allValidPositions;
    std::vector<Point> originalEnemyPositions;
    
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            char cell = map.layout[y][x];
            Point pos(x, y);
            
            if (cell == 'E') {
                originalEnemyPositions.push_back(pos);
            }
            
            if (isValidEnemyPosition(pos, map, oldPlayer)) {
                allValidPositions.push_back(pos);
            }
        }
    }
    
    std::vector<Point> enemyPositions;
    
    for (const auto& pos : originalEnemyPositions) {
        enemyPositions.push_back(pos);
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::shuffle(allValidPositions.begin(), allValidPositions.end(), gen);
    
    for (const auto& pos : allValidPositions) {
        if (std::find(enemyPositions.begin(), enemyPositions.end(), pos) != enemyPositions.end()) {
            continue;
        }
        
        if (oldPlayer) {
            Point playerPos = oldPlayer->getPosition();
            int distance = abs(pos.x - playerPos.x) + abs(pos.y - playerPos.y);
            if (distance < 5) {
                continue;
            }
        }
        
        enemyPositions.push_back(pos);
        
        if (enemyPositions.size() >= static_cast<size_t>(adjustedEnemyCount)) {
            break;
        }
    }
    
    if (enemyPositions.size() < static_cast<size_t>(adjustedEnemyCount)) {
        for (const auto& pos : allValidPositions) {
            if (std::find(enemyPositions.begin(), enemyPositions.end(), pos) == enemyPositions.end()) {
                enemyPositions.push_back(pos);
                if (enemyPositions.size() >= static_cast<size_t>(adjustedEnemyCount)) {
                    break;
                }
            }
        }
    }
    
    if (enemyPositions.size() > static_cast<size_t>(adjustedEnemyCount)) {
        enemyPositions.resize(adjustedEnemyCount);
    }
    
    // РАСПРЕДЕЛЕНИЕ ТИПОВ ТАНКОВ В ЗАВИСИМОСТИ ОТ УРОВНЯ
    std::vector<EnemyTankType> tankTypes;
    
    for (size_t i = 0; i < enemyPositions.size(); i++) {
        EnemyTankType tankType = getRandomTankType(level, gen);
        tankTypes.push_back(tankType);
    }
    
    // СОЗДАЕМ ВРАЖЕСКИЕ ТАНКИ С РАЗНЫМИ ТИПАМИ
    for (size_t i = 0; i < enemyPositions.size(); i++) {
        AIBehavior behavior = getAIBehaviorForDifficulty(enemyDifficulty, gen);
        EnemyTankType tankType = tankTypes[i];
        
        objects.emplace_back(new EnemyTank(enemyPositions[i], behavior, enemyDifficulty, tankType));
        enemyCount++;
    }
    
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            char cell = map.layout[y][x];
            Point pos(x, y);
            
            if (oldPlayer && pos.x == oldPlayer->getPosition().x && pos.y == oldPlayer->getPosition().y) {
                continue;
            }
            
            if (std::find(enemyPositions.begin(), enemyPositions.end(), pos) != enemyPositions.end()) {
                continue;
            }
            
            switch (cell) {
                case '#': // Кирпичная стена
                    objects.emplace_back(new Obstacle(pos, ObstacleType::BRICK));
                    obstacleCount++;
                    break;
                case 'X': // Стальная стена
                    objects.emplace_back(new Obstacle(pos, ObstacleType::STEEL));
                    obstacleCount++;
                    break;
                case '~': // Вода
                    objects.emplace_back(new Obstacle(pos, ObstacleType::WATER));
                    obstacleCount++;
                    break;
                case '*': // Лес
                    objects.emplace_back(new Obstacle(pos, ObstacleType::FOREST));
                    obstacleCount++;
                    break;
                case '^':
                case 'v':
                case '<':
                case '>':
                    if (oldPlayer && (y != oldPlayer->getPosition().y || x != oldPlayer->getPosition().x)) {
                        oldPlayer->setPosition(pos);
                        Direction dir = Direction::UP;
                        if (cell == 'v') dir = Direction::DOWN;
                        else if (cell == '<') dir = Direction::LEFT;
                        else if (cell == '>') dir = Direction::RIGHT;
                        oldPlayer->rotate(dir);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

EnemyTankType MapManager::getRandomTankType(int level, std::mt19937& gen) const {
    std::uniform_int_distribution<> typeDist(0, 100);
    int typeRoll = typeDist(gen);
    
    if (level >= 2) {
        if (typeRoll < 50) { // 50% - обычные танки
            return EnemyTankType::BASIC;
        } else if (typeRoll < 70) { // 20% - быстрые танки
            return EnemyTankType::FAST;
        } else if (typeRoll < 85) { // 15% - танки с уроном
            return EnemyTankType::DAMAGE;
        } else { // 15% - бронированные танки
            return EnemyTankType::ARMORED;
        }
    } else {
        // На первом уровне только обычные танки
        return EnemyTankType::BASIC;
    }
    
    // На высоких уровнях увеличиваем шанс появления специальных танков
    if (level >= 4) {
        typeRoll = typeDist(gen); // Перебрасываем кубик
        if (typeRoll < 30) { // 30% - обычные
            return EnemyTankType::BASIC;
        } else if (typeRoll < 55) { // 25% - быстрые
            return EnemyTankType::FAST;
        } else if (typeRoll < 75) { // 20% - с уроном
            return EnemyTankType::DAMAGE;
        } else { // 25% - бронированные
            return EnemyTankType::ARMORED;
        }
    }
    
    return EnemyTankType::BASIC;
}

bool MapManager::isValidEnemyPosition(const Point& pos, const MapInfo& map, const PlayerTank* player) const {
    // Проверяем границы
    if (pos.x <= 0 || pos.x >= map.width - 1 ||
        pos.y <= 0 || pos.y >= map.height - 1 ||
        pos.x == map.width / 2 || pos.y == map.height - 3) {
        
        return false;
    }
    
    if (player) {
        Point playerPos = player->getPosition();
        int distance = abs(pos.x - playerPos.x) + abs(pos.y - playerPos.y);
        if (distance < 4) {
            return false;
        }
    }
    
    char cell = map.layout[pos.y][pos.x];
    switch (cell) {
        case '#':
        case 'X': 
        case '~':
        case 'E':
            return false;
        case '*':
        case ' ':
        case '^':
        case 'v':
        case '<':
        case '>':
            return true;
        default:
            return false;
    }
}


int MapManager::countEnemiesInLayout(const std::vector<std::vector<char>>& layout) const {
    int count = 0;
    for (const auto& row : layout) {
        for (char cell : row) {
            if (cell == 'E') {
                count++;
            }
        }
    }
    return count;
}

int MapManager::calculateAdjustedEnemyCount(int baseCount, int level) const {
    int adjusted = baseCount + (level - 1) * 2;
    
    int maxEnemies = 15;
    return std::min(adjusted, maxEnemies);
}

int MapManager::calculateEnemyDifficulty(int level) const {
    if (level <= 3) return 1;
    if (level <= 6) return 2;
    return 3;
}

AIBehavior MapManager::getAIBehaviorForDifficulty(int difficulty, std::mt19937& gen) const {
    std::uniform_real_distribution<> dist(0.0, 1.0);
    double random = dist(gen);
    
    switch (difficulty) {
        case 1:
            if (random < 0.7) return AIBehavior::RANDOM;
            if (random < 0.9) return AIBehavior::DEFENSIVE;
            return AIBehavior::AGGRESSIVE;
            
        case 2:
            if (random < 0.4) return AIBehavior::RANDOM;
            if (random < 0.7) return AIBehavior::DEFENSIVE;
            return AIBehavior::AGGRESSIVE;
            
        case 3:
            if (random < 0.2) return AIBehavior::RANDOM;
            if (random < 0.5) return AIBehavior::DEFENSIVE;
            return AIBehavior::AGGRESSIVE;
            
        default:
            return AIBehavior::RANDOM;
    }
}
