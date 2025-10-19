#include "InputHandler.h"

InputHandler::InputHandler() {
    keyBindings['w'] = Command::MOVE_UP;
    keyBindings['W'] = Command::MOVE_UP;
    keyBindings['s'] = Command::MOVE_DOWN;
    keyBindings['S'] = Command::MOVE_DOWN;
    keyBindings['a'] = Command::MOVE_LEFT;
    keyBindings['A'] = Command::MOVE_LEFT;
    keyBindings['d'] = Command::MOVE_RIGHT;
    keyBindings['D'] = Command::MOVE_RIGHT;
    keyBindings[' '] = Command::FIRE;
    keyBindings['f'] = Command::FIRE;
    keyBindings['F'] = Command::FIRE;
    keyBindings['p'] = Command::PAUSE;
    keyBindings['P'] = Command::PAUSE;
    keyBindings['m'] = Command::MENU;
    keyBindings['M'] = Command::MENU;
    keyBindings['\n'] = Command::CONFIRM; // Enter
    keyBindings[27] = Command::BACK; // Escape
    keyBindings['q'] = Command::EXIT;
    keyBindings['Q'] = Command::EXIT;

    // Привязки для кириллицы (UTF-8)
    utf8KeyBindings["ц"] = Command::MOVE_UP;      // w -> ц
    utf8KeyBindings["Ц"] = Command::MOVE_UP;      // W -> Ц
    utf8KeyBindings["ы"] = Command::MOVE_DOWN;    // s -> ы
    utf8KeyBindings["Ы"] = Command::MOVE_DOWN;    // S -> Ы
    utf8KeyBindings["ф"] = Command::MOVE_LEFT;    // a -> ф
    utf8KeyBindings["Ф"] = Command::MOVE_LEFT;    // A -> Ф
    utf8KeyBindings["в"] = Command::MOVE_RIGHT;   // d -> в
    utf8KeyBindings["В"] = Command::MOVE_RIGHT;   // D -> В
    utf8KeyBindings["а"] = Command::FIRE;         // f -> а
    utf8KeyBindings["А"] = Command::FIRE;         // F -> А
    utf8KeyBindings["з"] = Command::PAUSE;        // p -> з
    utf8KeyBindings["З"] = Command::PAUSE;        // P -> З
    utf8KeyBindings["ь"] = Command::MENU;         // m -> ь
    utf8KeyBindings["Ь"] = Command::MENU;         // M -> Ь
    utf8KeyBindings["й"] = Command::EXIT;         // q -> й
    utf8KeyBindings["Й"] = Command::EXIT;         // Q -> Й
}

Command InputHandler::waitForCommand() {
    std::string input = PlatformUtils::readUTF8Char();
    
#ifdef _WIN32
    if (input.length() == 1) {
        int key = static_cast<unsigned char>(input[0]);
        
        if (key == 0 || key == 224) {
            int specialKey = PlatformUtils::readUTF8Char()[0];
            switch (specialKey) {
                case 72: // Стрелка вверх
                    return Command::MOVE_UP;
                case 80: // Стрелка вниз
                    return Command::MOVE_DOWN;
                case 75: // Стрелка влево
                    return Command::MOVE_LEFT;
                case 77: // Стрелка вправо
                    return Command::MOVE_RIGHT;
                default:
                    return Command::NONE;
            }
        }
        
        auto it = keyBindings.find(key);
        if (it != keyBindings.end()) {
            return it->second;
        }
    }
#else
    if (input.length() == 1 && static_cast<unsigned char>(input[0]) == 27) {
        if (PlatformUtils::kbhit()) {
            std::string secondChar = PlatformUtils::readUTF8Char();
            if (secondChar == "[") {
                if (PlatformUtils::kbhit()) {
                    std::string thirdChar = PlatformUtils::readUTF8Char();
                    if (thirdChar.length() == 1) {
                        switch (static_cast<unsigned char>(thirdChar[0])) {
                            case 65: // Стрелка вверх
                                return Command::MOVE_UP;
                            case 66: // Стрелка вниз
                                return Command::MOVE_DOWN;
                            case 68: // Стрелка влево
                                return Command::MOVE_LEFT;
                            case 67: // Стрелка вправо
                                return Command::MOVE_RIGHT;
                            default:
                                return Command::NONE;
                        }
                    }
                }
            }
        }
        return Command::BACK;
    }
    
    if (input.length() == 1) {
        int key = static_cast<unsigned char>(input[0]);
        auto it = keyBindings.find(key);
        if (it != keyBindings.end()) {
            return it->second;
        }
    }
#endif
    auto utf8It = utf8KeyBindings.find(input);
    if (utf8It != utf8KeyBindings.end()) {
        return utf8It->second;
    }
    
    return Command::NONE;
}

void InputHandler::remapKey(int keyCode, Command command) {
    keyBindings[keyCode] = command;
}

void InputHandler::remapUTF8Key(const std::string& utf8Char, Command command) {
    utf8KeyBindings[utf8Char] = command;
}

int InputHandler::getKeyCode(char c) {
    return static_cast<int>(c);
}
