// src/utils/Utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <random>
#include "Constants.h"

namespace Utils {
    // Random number generation
    int randomInt(int min, int max);
    double randomDouble(double min, double max);
    bool randomBool();
    
    // Math utilities
    int clamp(int value, int min, int max);
    double clamp(double value, double min, double max);
    int lerp(int a, int b, double t);
    double lerp(double a, double b, double t);
    
    // Direction utilities
    Constants::Direction getOppositeDirection(Constants::Direction dir);
    bool isHorizontalDirection(Constants::Direction dir);
    bool isVerticalDirection(Constants::Direction dir);
    void getDirectionOffset(Constants::Direction dir, int& dx, int& dy);
    
    // String utilities
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    
    // File utilities
    bool fileExists(const std::string& filename);
    std::string readFileToString(const std::string& filename);
    bool writeStringToFile(const std::string& filename, const std::string& content);
    
    // Game-specific utilities
    bool isValidPosition(int x, int y, int width, int height);
    int manhattanDistance(int x1, int y1, int x2, int y2);
    double euclideanDistance(int x1, int y1, int x2, int y2);
}

#endif