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
    auto [width, height] = getTerminalSize();
    return width >= minWidth && height >= minHeight;
}

void PlatformUtils::showResizeMessage(int requiredWidth, int requiredHeight) {
    auto [currentWidth, currentHeight] = getTerminalSize();
    
    clearScreen();
    std::cout << "============================================\n";
    std::cout << "        НЕДОСТАТОЧНЫЙ РАЗМЕР ТЕРМИНАЛА      \n";
    std::cout << "============================================\n\n";
    
    std::cout << "Текущий размер: " << currentWidth << "x" << currentHeight << "\n";
    std::cout << "Требуемый размер: " << requiredWidth << "x" << requiredHeight << "\n\n";
    
    std::cout << "Пожалуйста, увеличьте размер окна терминала\n";
    std::cout << "и перезапустите игру.\n\n";
    
    std::cout << "Нажмите enter для выхода...";
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
    WORD attributes = 0;
    
    switch (color) {
        case Color::RED:
            attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case Color::GREEN:
            attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case Color::YELLOW:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case Color::BLUE:
            attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::MAGENTA:
            attributes = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::CYAN:
            attributes = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case Color::WHITE:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        default:
            attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            break;
    }
    
    SetConsoleTextAttribute(hConsole, attributes);
#else
    const char* colorCode = "";
    switch (color) {
        case Color::RED:
            colorCode = "\033[91m";
            break;
        case Color::GREEN:
            colorCode = "\033[92m";
            break;
        case Color::YELLOW:
            colorCode = "\033[93m";
            break;
        case Color::BLUE:
            colorCode = "\033[94m";
            break;
        case Color::MAGENTA:
            colorCode = "\033[95m";
            break;
        case Color::CYAN:
            colorCode = "\033[96m";
            break;
        case Color::WHITE:
            colorCode = "\033[97m";
            break;
        default:
            colorCode = "\033[0m";
            break;
    }
    std::cout << colorCode;
#endif
}

void PlatformUtils::resetColor() {
#ifdef _WIN32
    setColor(Color::DEFAULT);
#else
    std::cout << "\033[0m";
#endif
}

