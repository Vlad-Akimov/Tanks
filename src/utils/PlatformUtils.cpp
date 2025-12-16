/**
 * @file PlatformUtils.cpp
 * @author Vld251
 * @brief Implementation of cross-platform console utilities.
 * @version 0.1
 * @date 2025-12-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <iostream>
#include "PlatformUtils.h"

#ifdef _WIN32
    HANDLE PlatformUtils::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    struct termios PlatformUtils::oldt;
#endif

void PlatformUtils::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#endif
#include <cstring>

std::pair<int, int> PlatformUtils::getTerminalSize() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return {width, height};
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return {w.ws_col, w.ws_row};
#endif
}

bool PlatformUtils::isTerminalSizeValid(int minWidth, int minHeight) {
    auto terminalSize = PlatformUtils::getTerminalSize();
    int width = terminalSize.first;
    int height = terminalSize.second;
    return width >= minWidth && height >= minHeight;
}

void PlatformUtils::showResizeMessage(int requiredWidth, int requiredHeight) {
    auto terminalSize = PlatformUtils::getTerminalSize();
    int currentWidth = terminalSize.first;
    int currentHeight = terminalSize.second;
    
    clearScreen();
    std::cout << "============================================\n";
    std::cout << "        TERMINAL SIZE TOO SMALL            \n";
    std::cout << "============================================\n\n";
    
    std::cout << "Current size: " << currentWidth << "x" << currentHeight << "\n";
    std::cout << "Required size: " << requiredWidth << "x" << requiredHeight << "\n\n";

    std::cout << "Please increase the terminal window size\n";
    std::cout << "and restart the game.\n\n";

    std::cout << "Press enter to exit...";
    std::cin.get();
}

void PlatformUtils::setCursorPosition(int x, int y) {
#ifdef _WIN32
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
#else
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
#endif
}

bool PlatformUtils::kbhit() {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    int ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    
    return false;
#endif
}

std::string PlatformUtils::readUTF8Char() {
    std::string result;
    
#ifdef _WIN32
    int ch = _getch();
    
    // Если это начало UTF-8 последовательности
    if ((ch & 0xE0) == 0xC0) { // 2-byte sequence
        result += static_cast<char>(ch);
        result += static_cast<char>(_getch());
    }
    else if ((ch & 0xF0) == 0xE0) { // 3-byte sequence
        result += static_cast<char>(ch);
        result += static_cast<char>(_getch());
        result += static_cast<char>(_getch());
    }
    else if ((ch & 0xF8) == 0xF0) { // 4-byte sequence
        result += static_cast<char>(ch);
        result += static_cast<char>(_getch());
        result += static_cast<char>(_getch());
        result += static_cast<char>(_getch());
    }
    else {
        // Одиночный ASCII символ
        result += static_cast<char>(ch);
    }
#else
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    // Читаем первый байт
    int firstByte = getchar();
    if (firstByte == EOF) {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return result;
    }
    
    result += static_cast<char>(firstByte);
    
    // Определяем длину UTF-8 последовательности по первому байту
    int numBytes = 1;
    if ((firstByte & 0xE0) == 0xC0) numBytes = 2;
    else if ((firstByte & 0xF0) == 0xE0) numBytes = 3;
    else if ((firstByte & 0xF8) == 0xF0) numBytes = 4;
    
    // Читаем оставшиеся байты
    for (int i = 1; i < numBytes; ++i) {
        if (!kbhit()) {
            usleep(1000);
        }
        int nextByte = getchar();
        if (nextByte != EOF) {
            result += static_cast<char>(nextByte);
        }
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    
    return result;
}

void PlatformUtils::sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void PlatformUtils::setColor(Color color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    // Коды цветов для Unix-терминалов
    const char* colors[] = {
        "\033[30m", "\033[31m", "\033[32m", "\033[33m",
        "\033[34m", "\033[35m", "\033[36m", "\033[37m",
        "", "\033[39m"
    };
    std::cout << colors[color];
#endif
}

void PlatformUtils::resetColor() {
#ifdef _WIN32
    setColor(Color::DEFAULT);
#else
    std::cout << "\033[0m";
#endif
}

bool PlatformUtils::supportsUnicode() {
#ifdef _WIN32
    // Для Windows проверяем кодовую страницу
    return GetConsoleOutputCP() == 65001; // UTF-8
#else
    // Для Linux/Mac обычно поддерживается
    return true;
#endif
}

void PlatformUtils::setBackgroundColor(Color color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Для Windows устанавливаем атрибуты текста с нужным фоном
    // Черный фон соответствует 0 в старших 4 битах
    SetConsoleTextAttribute(hConsole, (color << 4) | (GetConsoleTextAttribute(hConsole) & 0x0F));
#else
    // Коды цветов фона для Unix-терминалов
    const char* bgColors[] = {
        "\033[40m", "\033[41m", "\033[42m", "\033[43m",
        "\033[44m", "\033[45m", "\033[46m", "\033[47m",
        "", "\033[49m"
    };
    std::cout << bgColors[color];
#endif
}

void PlatformUtils::resetBackgroundColor() {
#ifdef _WIN32
    // Для Windows сбрасываем к стандартным атрибутам
    setColor(Color::DEFAULT);
#else
    std::cout << "\033[49m"; // Сброс цвета фона
#endif
}

bool PlatformUtils::checkUnicodeSupport() {
#ifdef _WIN32
    // Для Windows проверяем кодовую страницу и шрифт терминала
    UINT codePage = GetConsoleOutputCP();
    if (codePage != 65001) { // UTF-8
        return false;
    }
    
    // Дополнительная проверка поддержки Unicode символов
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    if (GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo)) {
        // Проверяем, что используется шрифт, поддерживающий Unicode
        return (wcscmp(fontInfo.FaceName, L"Consolas") == 0 || 
                wcscmp(fontInfo.FaceName, L"Lucida Console") == 0 ||
                wcscmp(fontInfo.FaceName, L"DejaVu Sans Mono") == 0);
    }
    return false;
#else
    // Для Linux/Mac проверяем переменные окружения
    const char* term = getenv("TERM");
    const char* lang = getenv("LANG");
    
    if (term && (strstr(term, "xterm") != nullptr || 
                 strstr(term, "linux") != nullptr ||
                 strstr(term, "vt100") != nullptr)) {
        if (lang && strstr(lang, "UTF-8") != nullptr) {
            return true;
        }
    }
    return false;
#endif
}
