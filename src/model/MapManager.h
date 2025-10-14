#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <string>
#include <vector>
#include <filesystem>

struct MapInfo {
    std::string filename;
    std::string displayName;
    int width;
    int height;
    std::string description;
};

class MapManager {
private:
    std::vector<MapInfo> maps;
    std::string mapsDirectory;

public:
    MapManager(const std::string& directory = "maps");
    bool loadMaps();
    void createDefaultMaps();
    const std::vector<MapInfo>& getMaps() const;
    const MapInfo& getMap(int index) const;
    int getMapCount() const;
    bool isValidMapIndex(int index) const;
};

#endif // MAPMANAGER_H
