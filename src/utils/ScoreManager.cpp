#include "ScoreManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

ScoreManager::ScoreManager() : scoreFile("highscores.txt") {
    loadScores();
}

ScoreManager::ScoreManager(const std::string& filename) : scoreFile(filename) {
    loadScores();
}

void ScoreManager::addScore(int score) {
    if (score <= 0) return;
    
    std::cout << "Добавление счета в таблицу рекордов: " << score << std::endl;
    
    // Проверяем, является ли счет рекордом
    if (!isHighScore(score)) {
        std::cout << "Счет " << score << " не является рекордом." << std::endl;
        return;
    }
    
    // Добавляем счет и сортируем
    highScores.push_back(score);
    std::sort(highScores.rbegin(), highScores.rend());
    
    highScores.erase(std::unique(highScores.begin(), highScores.end()), highScores.end());
    
    // Ограничиваем размер
    if (highScores.size() > MAX_SCORES) {
        highScores.resize(MAX_SCORES);
    }
    
    saveScores();
    
    std::cout << "Обновленная таблица рекордов: ";
    for (int s : highScores) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
}

bool ScoreManager::isHighScore(int score) const {
    if (highScores.empty()) return true;
    if (highScores.size() < MAX_SCORES) return true;
    
    return score > highScores.back();
}

std::vector<int> ScoreManager::getHighScores() const {
    return highScores;
}

void ScoreManager::saveScores() {
    std::ofstream file(scoreFile);
    if (file.is_open()) {
        for (int score : highScores) {
            file << score << std::endl;
        }
        file.close();
        std::cout << "Рекорды сохранены в файл: " << scoreFile << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось открыть файл для сохранения рекордов: " << scoreFile << std::endl;
    }
}

void ScoreManager::loadScores() {
    highScores.clear();
    std::ifstream file(scoreFile);
    
    if (file.is_open()) {
        int score;
        while (file >> score) {
            if (score > 0) {
                highScores.push_back(score);
            }
        }
        file.close();
        std::cout << "Загружено рекордов: " << highScores.size() << std::endl;
    } else {
        std::cout << "Файл рекордов не найден, будет создан новый." << std::endl;
    }
    
    std::sort(highScores.rbegin(), highScores.rend());
    highScores.erase(std::unique(highScores.begin(), highScores.end()), highScores.end());
    
    if (highScores.size() > MAX_SCORES) {
        highScores.resize(MAX_SCORES);
    }
}

void ScoreManager::clearScores() {
    highScores.clear();
    saveScores();
    std::cout << "Таблица рекордов очищена." << std::endl;
}
