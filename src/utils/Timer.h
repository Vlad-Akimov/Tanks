// src/utils/Timer.h
#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double deltaTime;
    double timeScale;
    bool paused;

public:
    Timer();
    
    void start();
    void update();
    void pause();
    void resume();
    void reset();
    
    double getDeltaTime() const { return deltaTime; }
    double getElapsedTime() const;
    double getTimeScale() const { return timeScale; }
    void setTimeScale(double scale) { timeScale = scale; }
    bool isPaused() const { return paused; }
    
    // Utility methods for frame rate control
    void limitFPS(int targetFPS);
    double getFPS() const;

private:
    std::chrono::steady_clock::time_point getCurrentTime() const;
};

#endif