/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** PizzaOrder.hpp
*/

#pragma once

#include "Plazza.hpp"

const std::vector<std::string> PIZZAS = {"regina", "margarita", "americana", "fantasia"};
const std::vector<std::string> SIZES = {"S", "M", "L", "XL", "XXL"};

enum PizzaType {
    Regina = 1,
    Margarita = 2,
    Americana = 4,
    Fantasia = 8,
};

enum PizzaSize {
    S = 1,
    M = 2,
    L = 4,
    XL = 8,
    XXL = 16,
};

using std::size_t;

class PizzaOrder {
  private:
    size_t _orderId;
    size_t _clientId;
    size_t _totalPizzasOrdered;
    size_t _pizzaOrderIndex;
    PizzaType _type;
    PizzaSize _size;

    std::map<std::string, int> _ingredients;
    int _bakingTime = 0;
    float _timeMultiplier = 0;

  public:
    // ! Constructors:

    PizzaOrder() = default;

    /**
    ** @brief Construct a new Pizza Order object
    ** @param orderId
    ** @param clientId
    ** @param totalPizzasOrdered : total number of pizzas ordered by the client
    ** @param pizzaOrderIndex : index of the pizza in the order
    ** @param type : type of the pizza (Regina, Margarita, Americana, Fantasia)
    ** @param size : size of the pizza (S, M, L, XL, XXL)
    ** @param timeMultiplier
    **/
    PizzaOrder(size_t orderId, size_t clientId, size_t totalPizzasOrdered, size_t pizzaOrderIndex,
               PizzaType type, PizzaSize size, float timeMultiplier);

    virtual ~PizzaOrder() = default;

    // ! Getters:

    size_t getOrderId() const;
    size_t getClientId() const;
    size_t getTotalPizzasOrdered() const;
    size_t getPizzaOrderIndex() const;
    PizzaType getType() const;
    std::string getTypeString() const;
    PizzaSize getSize() const;
    std::string getSizeString() const;
    int getBakingTime() const;

    // ! Set ingredients based on pizza type

    void setIngredients();
    void setBakingTime();
    std::map<std::string, int> getIngredients()
    {
        return _ingredients;
    }


    // ! Overloads for stream operators:

    /**
    ** @brief Overload of the << operator (pack)
    ** @param os The output stream
    ** @param order The order to display
    **/
    friend std::ostream& operator<<(std::ostream& os, const PizzaOrder& order);

    /**
    ** @brief Overload of the >> operator (unpack)
    ** @param is The input stream
    ** @param order The order to fill
    ** @return The input stream
    **/
    friend std::istream& operator>>(std::istream& is, PizzaOrder& order);
};

PizzaSize getPizzaSizeFromString(std::string pizzaSizeStr);
PizzaType getPizzaTypeFromString(std::string pizzaTypeStr);
