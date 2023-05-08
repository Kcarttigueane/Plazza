/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getPizzaTypeFromString.cpp
*/

#include "../include/PizzaOrder.hpp"

PizzaSize getPizzaSizeFromString(std::string pizzaSizeStr)
{
    for (size_t i = 0; i < SIZES.size(); i++) {
        if (pizzaSizeStr == SIZES[i])
            return static_cast<PizzaSize>(i);
    }
    throw std::invalid_argument("Invalid pizza size");
}
