/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Ingredients.hpp
*/

#pragma once

#include <unistd.h>
#include <map>
#include <string>

class Ingredients {
  private:
    std::map<std::string, int> _stock;
    int _replenishmentTime;

    void initialize_stock();

  public:
    Ingredients(int replenishmentTime);

    virtual ~Ingredients(){};

    // ! Getters:
    std::string getIndividualStock() const;

    // ! Add a specific amount of an ingredient to the stock
    void addIngredient(const std::string& ingredient, int amount);

    // ! Remove a specific amount of an ingredient from the stock
    bool removeIngredient(const std::string& ingredient, int amount);

    // ! Replenish the stock every N seconds
    void replenishStock();

    void printStock();
};
