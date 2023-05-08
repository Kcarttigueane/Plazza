/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** UIManager.hpp
*/

#pragma once

#include <ncurses.h>
#include <string>

class UIManager {
  private:
    WINDOW* input_win;
    WINDOW* update_win;

  public:
    UIManager();

    ~UIManager();

    void init_windows();
    void destroy_windows();
    std::string get_user_input();
    void display_update(const std::string& update);
};
