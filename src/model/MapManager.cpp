#ifndef MAPMANAGER_CPP
#define MAPMANAGER_CPP

#include "MapManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

MapManager::MapManager(const std::string& directory) 
    : mapsDirectory(directory) {
    loadMaps();
}

bool MapManager::loadMaps() {
    maps.clear();
    
    try {
        // Создаем директорию, если она не существует
        std::filesystem::create_directories(mapsDirectory);
        
        // Ищем все .map файлы в директории
        for (const auto& entry : std::filesystem::directory_iterator(mapsDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".map") {
                MapInfo mapInfo;
                mapInfo.filename = entry.path().filename().string();
                mapInfo.displayName = entry.path().stem().string();
                
                // Читаем базовую информацию о карте из файла
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    std::string line;
                    if (std::getline(file, line)) {
                        std::istringstream iss(line);
                        iss >> mapInfo.width >> mapInfo.height;
                    }
                    file.close();
                }
                
                // Генерируем описание
                mapInfo.description = "Размер: " + std::to_string(mapInfo.width) + "x" + 
                                    std::to_string(mapInfo.height);
                
                maps.push_back(mapInfo);
            }
        }
        
        // Если карт нет, создаем несколько примеров
        if (maps.empty()) {
            createDefaultMaps();
        }
        
        std::cout << "Загружено карт: " << maps.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки карт: " << e.what() << std::endl;
        createDefaultMaps();
        return false;
    }
}

void MapManager::createDefaultMaps() {
    // Создаем несколько стандартных карт
    MapInfo map1;
    map1.filename = "classic.map";
    map1.displayName = "Классическая";
    map1.width = 40;
    map1.height = 20;
    map1.description = "Классическая карта для начинающих";
    maps.push_back(map1);
    
    MapInfo map2;
    map2.filename = "battlefield.map";
    map2.displayName = "Поле боя";
    map2.width = 50;
    map2.height = 25;
    map2.description = "Большое поле с укрытиями";
    maps.push_back(map2);
    
    MapInfo map3;
    map3.filename = "fortress.map";
    map3.displayName = "Крепость";
    map3.width = 35;
    map3.height = 30;
    map3.description = "Защищенная крепость со стенами";
    maps.push_back(map3);
}

const std::vector<MapInfo>& MapManager::getMaps() const {
    return maps;
}

const MapInfo& MapManager::getMap(int index) const {
    return maps[index];
}

int MapManager::getMapCount() const {
    return maps.size();
}

bool MapManager::isValidMapIndex(int index) const {
    return index >= 0 && index < static_cast<int>(maps.size());
}

#endif // MAPMANAGER_CPP
