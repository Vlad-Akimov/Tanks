// src/utils/Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    const int SCREEN_WIDTH = 80;
    const int SCREEN_HEIGHT = 24;
    const int GAME_FIELD_WIDTH = 40;
    const int GAME_FIELD_HEIGHT = 20;
    
    enum class Direction { UP, DOWN, LEFT, RIGHT };
    enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, SETTINGS };
    enum class BonusType { SHIELD, DOUBLE_FIRE, SPEED_BOOST, LIFE_UP };
    enum class ObstacleType { BRICK, STEEL, WATER, FOREST };
}

#endif