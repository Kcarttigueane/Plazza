/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getPizzaSizeFromString.cpp
*/

#include "../include/PizzaOrder.hpp"

PizzaType getPizzaTypeFromString(std::string pizzaTypeStr)
{
    for (size_t i = 0; i < PIZZAS.size(); i++) {
        if (pizzaTypeStr == PIZZAS[i])
            return static_cast<PizzaType>(i);
    }
    throw std::invalid_argument("Invalid pizza type");
}
