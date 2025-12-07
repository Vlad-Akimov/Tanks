/**
@mainpage Документация проекта Tanks Game

@section overview Обзор проекта
Tanks Game - консольная игра в танки, написанная на C++ с использованием объектно-ориентированного подхода.

@section structure Структура проекта

@subsection folder_structure Структура папок

- **src/controller/** - Контроллеры игры и обработка ввода
- **src/model/** - Игровые объекты и логика
- **src/utils/** - Вспомогательные утилиты
- **src/view/** - Отображение игры
- **resources/** - Ресурсы игры

@subsection class_structure Структура классов

- **GameObject** - Базовый класс всех игровых объектов
- **Tank** и его наследники (PlayerTank, EnemyTank)
- **GameWorld** - Игровой мир
- **GameController** - Основной контроллер игры

@section build_instructions Инструкции по сборке

```bash
mkdir build && cd build
cmake ..
make

@section usage Использование
Запуск игры:

```bash
./TanksGame
```

Управление:

- Стрелки - движение

- Пробел - стрельба

- ESC - выход

@section authors Авторы

- Vld251

@date Дата последнего обновления: 8.12.2025

<https://github.com/Vlad-Akimov/Tanks>

USE_MDFILE_AS_MAINPAGE = docs/mainpage.md
