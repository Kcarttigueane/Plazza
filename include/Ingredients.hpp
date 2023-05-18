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
    explicit Ingredients(int replenishmentTime);

    virtual ~Ingredients() = default;

    [[nodiscard]] std::string getIndividualStock() const;

    [[nodiscard]] std::map<std::string, int> getStock() const { return _stock; }

    void addIngredient(const std::string& ingredient, int amount);

    bool removeIngredient(const std::string& ingredient, int amount);

    [[nodiscard]] bool isStockEmpty() const;

    void printStock();
};
