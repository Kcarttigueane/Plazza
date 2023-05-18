/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getPizzaTypeFromString.cpp
*/

#include "../include/PizzaOrder.hpp"

PizzaType getPizzaTypeFromString(const std::string& pizzaTypeStr)
{
    auto it = PIZZA_TYPES.find(pizzaTypeStr);

    if (it != PIZZA_TYPES.end()) {
        return it->second;
    }
    throw std::invalid_argument("Invalid pizza type");
}
