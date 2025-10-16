#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <string>
#include <vector>
#include <filesystem>
#include "../model/GameObject.h"
#include "../model/GameWorld.h"
#include "../model/Obstacle.h"

struct MapInfo {
    std::string filename;
    std::string displayName;
    std::string description;
    int width;
    int height;
    std::vector<std::vector<char>> layout;
};

class MapManager {
private:
    std::vector<MapInfo> maps;
    std::string mapsDirectory;
    void saveMapToFile(const MapInfo& map);

public:
    MapManager(const std::string& directory = "maps");
    bool loadMaps();
    void createDefaultMaps();
    const std::vector<MapInfo>& getMaps() const;
    const MapInfo& getMap(int index) const;
    int getMapCount() const;
    bool isValidMapIndex(int index) const;
    
    void createWorldFromMap(GameWorld& world, const MapInfo& map);
};

#endif // MAPMANAGER_H
