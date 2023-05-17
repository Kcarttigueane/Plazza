/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** getPizzaSizeFromString.cpp
*/

#include "PizzaOrder.hpp"

PizzaType getPizzaTypeFromString(const std::string& pizzaTypeStr)
{
    if (pizzaTypeStr == "regina")
        return PizzaType::Regina;
    else if (pizzaTypeStr == "margarita")
        return PizzaType::Margarita;
    else if (pizzaTypeStr == "americana")
        return PizzaType::Americana;
    else if (pizzaTypeStr == "fantasia")
        return PizzaType::Fantasia;
    else
        throw std::invalid_argument("Invalid pizza type");
}
