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
    size_t _replenishmentTime;

    void initialize_stock();

  public:
    explicit Ingredients(size_t replenishmentTime);

    virtual ~Ingredients() = default;

    [[nodiscard]] std::string getIndividualStock() const;

    void addIngredient(const std::string& ingredient, int amount);

    bool removeIngredient(const std::string& ingredient, int amount);

    void replenishStock();

    [[nodiscard]] bool isStockEmpty() const;

    void printStock();
};
