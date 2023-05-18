/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getPizzaSizeFromString.cpp
*/

#include "PizzaOrder.hpp"

PizzaSize getPizzaSizeFromString(const std::string& pizzaSizeStr)
{
    auto it = PIZZA_SIZES.find(pizzaSizeStr);

    if (it != PIZZA_SIZES.end()) {
        return it->second;
    }
    throw std::invalid_argument("Invalid pizza size");
}
