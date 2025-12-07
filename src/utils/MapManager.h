/**
 * @file MapManager.h
 * @author Vld251
 * @brief Manages game maps, level generation, and procedural content.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <string>
#include <vector>
#include <filesystem>
#include "../model/GameObject.h"
#include "../model/GameWorld.h"
#include "../model/Obstacle.h"
#include "../model/EnemyTank.h"

/**
 * @brief Structure containing map information and layout.
 */
struct MapInfo {
    std::string filename;               ///< Map file name
    std::string displayName;            ///< Display name for UI
    std::string description;            ///< Map description
    int width;                          ///< Map width in cells
    int height;                         ///< Map height in cells
    std::vector<std::vector<char>> layout;  ///< 2D grid representing map
};

/**
 * @brief Class for managing game maps and level generation.
 * 
 * Handles loading maps from files, creating default maps,
 * and converting maps to game world objects. Supports procedural
 * enemy placement and difficulty scaling.
 */
class MapManager {
private:
    std::vector<MapInfo> maps;          ///< List of available maps
    std::string mapsDirectory;          ///< Directory containing map files
    
    /**
     * @brief Saves map to file.
     * @param map Map information to save.
     * @returns None
     */
    void saveMapToFile(const MapInfo& map);
    
    /**
     * @brief Counts enemies in map layout.
     * @param layout 2D map layout.
     * @return Number of enemy markers found.
     */
    int countEnemiesInLayout(const std::vector<std::vector<char>>& layout) const;
    
    /**
     * @brief Calculates adjusted enemy count based on level.
     * @param baseCount Base enemy count from map.
     * @param level Current game level.
     * @return Adjusted enemy count.
     */
    int calculateAdjustedEnemyCount(int baseCount, int level) const;
    
    /**
     * @brief Calculates enemy difficulty based on level.
     * @param level Current game level.
     * @return Enemy difficulty level (1-3).
     */
    int calculateEnemyDifficulty(int level) const;
    
    /**
     * @brief Gets AI behavior based on difficulty level.
     * @param difficulty Difficulty level.
     * @param gen Random number generator.
     * @return AI behavior type.
     */
    AIBehavior getAIBehaviorForDifficulty(int difficulty, std::mt19937& gen) const;
    
    /**
     * @brief Checks if position is valid for enemy placement.
     * @param pos Position to check.
     * @param map Map information.
     * @param player Player tank object.
     * @return true if position valid for enemy, false otherwise.
     */
    bool isValidEnemyPosition(const Point& pos, const MapInfo& map, const PlayerTank* player) const;

    /**
     * @brief Gets random enemy tank type based on level.
     * @param level Current game level.
     * @param gen Random number generator.
     * @return Enemy tank type.
     */
    EnemyTankType getRandomTankType(int level, std::mt19937& gen) const;

public:
    /**
     * @brief Constructs a MapManager object.
     * @param directory Directory containing map files (default: "maps").
     * @returns None
     */
    MapManager(const std::string& directory = "maps");
    
    /**
     * @brief Loads all maps from directory.
     * @return true if maps loaded successfully, false otherwise.
     */
    bool loadMaps();
    
    /**
     * @brief Creates default maps if none exist.
     * @returns None
     */
    void createDefaultMaps();
    
    /**
     * @brief Gets list of all available maps.
     * @return Const reference to maps vector.
     */
    const std::vector<MapInfo>& getMaps() const;
    
    /**
     * @brief Gets specific map by index.
     * @param index Map index.
     * @return Const reference to MapInfo.
     */
    const MapInfo& getMap(int index) const;
    
    /**
     * @brief Gets total number of maps.
     * @return Number of available maps.
     */
    int getMapCount() const;
    
    /**
     * @brief Checks if map index is valid.
     * @param index Index to check.
     * @return true if index valid, false otherwise.
     */
    bool isValidMapIndex(int index) const;
    
    /**
     * @brief Creates game world from map layout.
     * @param world GameWorld to populate.
     * @param map Map to convert to game world.
     * @returns None
     */
    void createWorldFromMap(GameWorld& world, const MapInfo& map);
};

#endif // MAPMANAGER_H
