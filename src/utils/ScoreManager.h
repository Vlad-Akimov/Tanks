#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <vector>
#include <string>
#include <algorithm>

class ScoreManager {
private:
    std::vector<int> highScores;
    static const int MAX_SCORES = 3;
    std::string scoreFile;

public:
    ScoreManager();
    ScoreManager(const std::string& filename); // Новый конструктор
    void addScore(int score);
    std::vector<int> getHighScores() const;
    void saveScores();
    void loadScores();
    void clearScores();
    bool isHighScore(int score) const;
};

#endif // SCOREMANAGER_H