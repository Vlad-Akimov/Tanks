# Tanks

## Description

Tanks is a classic console-based arcade tank battle game developed as part of the Educational Practice in C++ course at Bauman Moscow State Technical University. The game features player-controlled tanks, enemy combatants, destructible obstacles, various power-ups, and a scoring system.

### Key features

+ Player tank control via keyboard
+ Different enemy tank types with unique behaviors
+ Destructible and indestructible obstacles
+ Bonus system (speed boost, armor upgrade, extra lives, etc.)
+ Dynamic explosion and visual effects system
+ Support for multiple game maps
+ Console rendering using ASCII graphics
+ Game configuration through settings files
+ Score saving and loading

### Controls

+ Arrow keys or WASD — move tank
+ Space or Enter — shoot
+ P — pause
+ ESC — exit game

***

> This project made for the "Educational practice on C++" course at Bauman Moscow State Technical University

## Requirements

The following must be installed on the device:

+ C++17
+ Cmake 3.10
+ Make

## Installation

Check the releases tab to catch the last release

Or, you can build project by yourself:

```bash
git clone https://github.com/Vlad-Akimov/Tanks
cd Tanks
mkdir build
cd build
cmake ..
make
```

Here we go!

To run the game type this from `build` folder:

```bash
./TanksGame
```
