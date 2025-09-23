// src/view/MenuView.h
#ifndef MENUVIEW_H
#define MENUVIEW_H

#include <vector>
#include <string>
#include "../utils/Constants.h"

class ConsoleRenderer;

struct MenuItem {
    std::string text;
    std::function<void()> action;
    bool enabled;
};

class MenuView {
private:
    ConsoleRenderer* renderer;
    std::vector<MenuItem> menuItems;
    int selectedIndex;
    std::string title;
    Constants::GameState menuType;

public:
    MenuView(Constants::GameState type, const std::string& menuTitle);
    ~MenuView();
    
    void addMenuItem(const std::string& text, std::function<void()> action, bool enabled = true);
    void removeMenuItem(int index);
    void clearMenuItems();
    
    void render();
    void handleInput(int key);
    void navigateUp();
    void navigateDown();
    void selectCurrentItem();
    
    void setTitle(const std::string& newTitle) { title = newTitle; }
    std::string getTitle() const { return title; }
    int getSelectedIndex() const { return selectedIndex; }
    int getItemCount() const { return menuItems.size(); }
    
    // Specific menu types
    static MenuView createMainMenu();
    static MenuView createPauseMenu();
    static MenuView createSettingsMenu();
    static MenuView createGameOverMenu(int score);
};

#endif