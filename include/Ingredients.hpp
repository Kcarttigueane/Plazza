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

    std::string getIndividualStock() const;

    void addIngredient(const std::string& ingredient, int amount);

    bool removeIngredient(const std::string& ingredient, int amount);

    void replenishStock();

    bool isStockEmpty() const;

    void printStock();
};
