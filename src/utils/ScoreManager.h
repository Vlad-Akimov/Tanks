/**
 * @file ScoreManager.h
 * @author Vld251
 * @brief Manages high scores, leaderboards, and player achievements.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <vector>
#include <string>
#include <algorithm>

/**
 * @brief Class for managing high scores and player achievements.
 * 
 * Handles loading, saving, and maintaining a high scores table.
 * Supports multiple high scores with automatic sorting and ranking.
 */
class ScoreManager {
private:
    std::vector<int> highScores;        ///< List of high scores (sorted descending)
    static const int MAX_SCORES = 3;    ///< Maximum number of high scores to keep
    std::string scoreFile;              ///< Path to high scores file

public:
    /**
     * @brief Constructs a ScoreManager with default score file.
     * @returns None
     */
    ScoreManager();
    
    /**
     * @brief Constructs a ScoreManager with custom score file.
     * @param filename Path to score file.
     * @returns None
     */
    ScoreManager(const std::string& filename);
    
    /**
     * @brief Adds a new score to high scores table.
     * @param score Score to add.
     * @returns None
     */
    void addScore(int score);
    
    /**
     * @brief Gets current high scores.
     * @return Vector of high scores (sorted descending).
     */
    std::vector<int> getHighScores() const;
    
    /**
     * @brief Saves high scores to file.
     * @returns None
     */
    void saveScores();
    
    /**
     * @brief Loads high scores from file.
     * @returns None
     */
    void loadScores();
    
    /**
     * @brief Clears all high scores.
     * @returns None
     */
    void clearScores();
    
    /**
     * @brief Checks if score qualifies as a high score.
     * @param score Score to check.
     * @return true if score is high score, false otherwise.
     */
    bool isHighScore(int score) const;
};

#endif // SCOREMANAGER_H
