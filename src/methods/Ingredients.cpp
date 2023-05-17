/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Ingredients.cpp
*/

#include "Ingredients.hpp"
#include "Plazza.hpp"

Ingredients::Ingredients(int replenishmentTime) : _replenishmentTime(replenishmentTime)
{
    initialize_stock();
}

void Ingredients::initialize_stock()
{
    _stock["doe"] = 5;
    _stock["tomato"] = 5;
    _stock["gruyere"] = 5;
    _stock["ham"] = 5;
    _stock["mushrooms"] = 5;
    _stock["steak"] = 5;
    _stock["eggplant"] = 5;
    _stock["goat_cheese"] = 5;
    _stock["chief_love"] = 5;
}

std::string Ingredients::getIndividualStock() const
{
    std::string stock;

    for (auto& ingredient : _stock) {
        stock += ingredient.first + "=" + std::to_string(ingredient.second) + "\n";
    }
    return stock;
}

void Ingredients::addIngredient(const std::string& ingredient, int amount)
{
    _stock[ingredient] += amount;
}

bool Ingredients::removeIngredient(const std::string& ingredient, int amount)
{
    if (_stock[ingredient] >= amount) {
        _stock[ingredient] -= amount;
        return true;
    }
    return false;
}

void Ingredients::replenishStock()
{
    for (auto& ingredient : _stock) {
        int currentStock = ingredient.second;
        int amountToAdd = std::min(5 - currentStock, 1);
        addIngredient(ingredient.first, amountToAdd);
    }
}

bool Ingredients::isStockEmpty() const
{
    for (auto& ingredient : _stock) {
        if (ingredient.second > 0) {
            return false;
        }
    }
    return true;
}

void Ingredients::printStock()
{
    for (auto& ingredient : _stock) {
        std::cout << ingredient.first << "=" << ingredient.second << std::endl;
    }
}
