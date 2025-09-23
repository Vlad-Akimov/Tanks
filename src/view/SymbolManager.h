// src/view/SymbolManager.h
#ifndef SYMBOLMANAGER_H
#define SYMBOLMANAGER_H

#include <map>
#include <string>
#include "../utils/Constants.h"

class SymbolManager {
private:
    std::map<Constants::Direction, char> tankSymbols;
    std::map<Constants::ObstacleType, char> obstacleSymbols;
    std::map<Constants::BonusType, char> bonusSymbols;
    std::map<std::string, char> uiSymbols;
    
    char projectileSymbol;
    char playerSymbol;
    char enemySymbol;
    
public:
    SymbolManager();
    ~SymbolManager() = default;
    
    void loadDefaultSymbols();
    void loadSymbolsFromFile(const std::string& filename);
    void saveSymbolsToFile(const std::string& filename);
    
    // Getters for symbols
    char getTankSymbol(Constants::Direction dir) const;
    char getObstacleSymbol(Constants::ObstacleType type) const;
    char getBonusSymbol(Constants::BonusType type) const;
    char getUISymbol(const std::string& name) const;
    char getProjectileSymbol() const { return projectileSymbol; }
    char getPlayerSymbol() const { return playerSymbol; }
    char getEnemySymbol() const { return enemySymbol; }
    
    // Setters for symbols
    void setTankSymbol(Constants::Direction dir, char symbol);
    void setObstacleSymbol(Constants::ObstacleType type, char symbol);
    void setBonusSymbol(Constants::BonusType type, char symbol);
    void setUISymbol(const std::string& name, char symbol);
    void setProjectileSymbol(char symbol) { projectileSymbol = symbol; }
    void setPlayerSymbol(char symbol) { playerSymbol = symbol; }
    void setEnemySymbol(char symbol) { enemySymbol = symbol; }
    
    // Utility methods
    char getDirectionalChar(char baseChar, Constants::Direction dir) const;
    int getColorCode(Constants::BonusType type) const;
    int getColorCode(Constants::ObstacleType type) const;
};

#endif