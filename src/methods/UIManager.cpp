/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** UIManager.cpp
*/

#include "UIManager.hpp"

UIManager::UIManager()
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    init_windows();
}

UIManager::~UIManager()
{
    destroy_windows();
    endwin();
}

void UIManager::init_windows()
{
    int height, width;
    getmaxyx(stdscr, height, width);

    // ! Create the command input window
    int input_window_height = height / 4;
    input_win = newwin(input_window_height, width, height - input_window_height, 0);
    box(input_win, 0, 0);
    mvwprintw(input_win, 0, 2, " Input ");
    wrefresh(input_win);

    // Create the update display window
    update_win = newwin(height - input_window_height, width, 0, 0);
    box(update_win, 0, 0);
    mvwprintw(update_win, 0, 2, " Updates ");
    wrefresh(update_win);
}

void UIManager::destroy_windows()
{
    delwin(input_win);
    delwin(update_win);
}

std::string UIManager::get_user_input()
{
    std::string input;
    int current_line = 1;

    while (true) {
        mvwprintw(input_win, current_line, 1, " -> ");
        wrefresh(input_win);
        int ch = wgetch(input_win);

        if (ch == 17) {
            return "QUIT";
        } else if (ch == KEY_ENTER || ch == '\n') {
            // Clear the current input line, including the command prompt
            // wmove(input_win, current_line, 1);
            // wclrtoeol(input_win);
            // wrefresh(input_win);

            // Move to the next line
            current_line++;
            if (current_line >= getmaxy(input_win)) {
                scroll(input_win);
                current_line--;
            }
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (!input.empty()) {
                input.pop_back();
            }
        } else {
            input.push_back(static_cast<char>(ch));
        }

        // Clear the current input line and redraw the input string
        wmove(input_win, current_line, 1);
        wclrtoeol(input_win);
        mvwprintw(input_win, current_line, 1, "> %s", input.c_str());
        wrefresh(input_win);
    }

    return input;
}

void UIManager::display_update(const std::string& update)
{
    int update_win_height, update_win_width;
    getmaxyx(update_win, update_win_height, update_win_width);
    mvwprintw(update_win, update_win_height - 2, 1, update.c_str());
    wrefresh(update_win);
}
