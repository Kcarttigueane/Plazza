/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getCurrentTimeString.cpp
*/

#include "Plazza.hpp"

#include <chrono>

std::string getCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&now_time);

    char time_str[100];
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);

    return {time_str};
}
