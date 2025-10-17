#ifndef MAPMANAGER_CPP
#define MAPMANAGER_CPP

#include "MapManager.h"
#include "../model/PlayerTank.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

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
        std::filesystem::create_directories(mapsDirectory);
        
        if (!std::filesystem::exists(mapsDirectory)) {
            std::cerr << "Директория не существует: " << mapsDirectory << std::endl;
            createDefaultMaps();
            return false;
        }
        
        // Ищем все .map файлы в директории
        for (const auto& entry : std::filesystem::directory_iterator(mapsDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".map") {
                std::cout << "Найден файл карты: " << entry.path() << std::endl;
                
                MapInfo mapInfo;
                mapInfo.filename = entry.path().filename().string();
                
                // Читаем информацию о карте из файла
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    std::string line;
                    
                    // Читаем displayName (первая строка)
                    if (std::getline(file, line)) {
                        mapInfo.displayName = line;
                        std::cout << "DisplayName: " << mapInfo.displayName << std::endl;
                    } else {
                        std::cerr << "Ошибка: файл карты пуст или поврежден: " << mapInfo.filename << std::endl;
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
                            std::cerr << "Ошибка чтения размеров карты из файла: " << mapInfo.filename << std::endl;
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
                            std::cout << "Пропущена строка неверной длины: " << line.length() 
                                      << " вместо " << mapInfo.width << " в файле " << mapInfo.filename << std::endl;
                        }
                    }
                    
                    file.close();
                    
                    // Проверяем корректность данных
                    if (mapInfo.layout.size() == static_cast<size_t>(mapInfo.height)) {
                        maps.push_back(mapInfo);
                        std::cout << "Успешно загружена карта: " << mapInfo.displayName 
                                  << " (" << linesRead << " строк)" << std::endl;
                    } else {
                        std::cerr << "Неверное количество строк в карте " << mapInfo.filename 
                                  << ": " << mapInfo.layout.size() << " вместо " << mapInfo.height << std::endl;
                    }
                } else {
                    std::cerr << "Не удалось открыть файл: " << entry.path() << std::endl;
                }
            }
        }
        
        // Если карт нет, создаем несколько примеров
        if (maps.empty()) {
            std::cout << "Карты не найдены, создаем карты по умолчанию..." << std::endl;
            createDefaultMaps();
        } else {
            std::cout << "Всего загружено карт: " << maps.size() << std::endl;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки карт: " << e.what() << std::endl;
        createDefaultMaps();
        return false;
    }
}

void MapManager::createDefaultMaps() {
    std::cout << "Создание карт по умолчанию..." << std::endl;
    
    // Создаем директорию, если она не существует
    std::filesystem::create_directories(mapsDirectory);
    
    // Карта 1: Классическая
    MapInfo map1;
    map1.filename = "classic.map";
    map1.displayName = "Классическая арена";
    map1.description = "Классическая карта для начинающих с балансом укрытий и открытых пространств";
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
    map2.displayName = "Крепость";
    map2.description = "Защищенная крепость со сложными укреплениями";
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
    map3.displayName = "Поле боя";
    map3.description = "Большое поле с укрытиями и стратегическими позициями";
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
    
    std::cout << "Создано карт по умолчанию: " << maps.size() << std::endl;
}

// Исправленный метод для сохранения карты в файл
void MapManager::saveMapToFile(const MapInfo& map) {
    // Правильное создание пути к файлу
    std::filesystem::path dirPath(mapsDirectory);
    std::filesystem::path filePath = dirPath / map.filename;
    
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
        std::cout << "Карта сохранена: " << filePath << std::endl;
    } else {
        std::cerr << "Ошибка сохранения карты: " << filePath << std::endl;
    }
}

const std::vector<MapInfo>& MapManager::getMaps() const {
    return maps;
}

const MapInfo& MapManager::getMap(int index) const {
    if (index >= 0 && index < static_cast<int>(maps.size())) {
        return maps[index];
    }
    throw std::out_of_range("Неверный индекс карты");
}

int MapManager::getMapCount() const {
    return maps.size();
}

bool MapManager::isValidMapIndex(int index) const {
    return index >= 0 && index < static_cast<int>(maps.size());
}

void MapManager::createWorldFromMap(GameWorld& world, const MapInfo& map) {
    std::cout << "Создание мира из карты: " << map.displayName << std::endl;
    
    // СОХРАНЯЕМ состояние игрока
    PlayerTank* oldPlayer = world.getPlayer();
    int savedScore = oldPlayer ? oldPlayer->getScore() : 0;
    int savedLives = oldPlayer ? oldPlayer->getLives() : 3;
    int savedHealth = oldPlayer ? oldPlayer->getHealth() : 3;
    int currentLevel = world.getCurrentLevel();
    
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
    
    // Создаем объекты из layout карты
    int enemyCount = 0;
    int obstacleCount = 0;
    
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            char cell = map.layout[y][x];
            Point pos(x, y);
            
            // Пропускаем позицию игрока
            if (oldPlayer && pos.x == oldPlayer->getPosition().x && pos.y == oldPlayer->getPosition().y) {
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
                case 'E': // Враг
                    {
                        AIBehavior behavior = AIBehavior::RANDOM;
                        auto enemy = std::make_unique<EnemyTank>(pos, behavior, currentLevel);
                        objects.push_back(std::move(enemy));
                        enemyCount++;
                    }
                    break;
                case '^': // Игрок (альтернативное расположение)
                case 'v':
                case '<':
                case '>':
                    // Если на карте указана позиция игрока, используем ее
                    if (oldPlayer && (y != oldPlayer->getPosition().y || x != oldPlayer->getPosition().x)) {
                        oldPlayer->setPosition(pos);
                        // Также устанавливаем направление based on symbol
                        Direction dir = Direction::UP;
                        if (cell == 'v') dir = Direction::DOWN;
                        else if (cell == '<') dir = Direction::LEFT;
                        else if (cell == '>') dir = Direction::RIGHT;
                        oldPlayer->rotate(dir);
                    }
                    break;
                default:
                    // Пустое пространство - ничего не создаем
                    break;
            }
        }
    }
    
    std::cout << "Успешно создано: " << enemyCount << " врагов, " 
              << obstacleCount << " препятствий" << std::endl;
    std::cout << "Уровень: " << currentLevel << " Счет: " << savedScore << std::endl;
}

#endif // MAPMANAGER_CPP
