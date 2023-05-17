/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** parsePizzaOrder.cpp
*/

#include "PizzaOrder.hpp"
#include "Reception.hpp"

#include "regex"

using std::string;

static void serializePizzaAndMessageQueue(Reception& reception,
                                          std::vector<std::pair<string, string>> pizzaOrders)
{
    size_t totalPizzasOrdered = pizzaOrders.size();

    size_t orderId = IDGenerator::generateID();
    size_t clientId = IDGenerator::generateID();

    for (size_t pizzaOrderIndex = 0; pizzaOrderIndex < totalPizzasOrdered; pizzaOrderIndex++) {
        PizzaType type = getPizzaTypeFromString(std::get<0>(pizzaOrders[pizzaOrderIndex]));
        PizzaSize size = getPizzaSizeFromString(std::get<1>(pizzaOrders[pizzaOrderIndex]));

        PizzaOrder pizzaOrder(orderId, clientId, totalPizzasOrdered, pizzaOrderIndex, type, size,
                              reception.getTimeMultiplier());

        reception.distributeOrder(pizzaOrder);
    }
}

static bool isPizzaOrderValid(string& type, string& size, int number)
{
    if (std::find(PIZZAS.begin(), PIZZAS.end(), type) == PIZZAS.end()) {
        std::cerr << "Error: Invalid pizza type" << std::endl;
        return false;
    }

    if (std::find(SIZES.begin(), SIZES.end(), size) == SIZES.end()) {
        std::cerr << "Error: Invalid pizza size" << std::endl;
        return false;
    }

    if (number <= 0) {
        std::cerr << "Error: Invalid pizza number" << std::endl;
        return false;
    }
    return true;
}

void parsePizzaOrder(string& input, Reception& reception)
{
    std::regex pattern("([a-zA-Z]+) (S|M|L|XL|XXL) x([0-9]+)(;|$)");
    std::smatch matches;

    std::vector<std::pair<string, string>> pizzaOrders;

    while (std::regex_search(input, matches, pattern)) {
        string type = matches[1].str();
        string size = matches[2].str();
        int number = std::stoi(matches[3].str());

        if (!isPizzaOrderValid(type, size, number)) {
            std::cerr << "Error: Invalid pizza order" << std::endl;
            return;
        }

        for (int i = 0; i < number; i++) {
            pizzaOrders.emplace_back(type, size);
        }

        input = matches.suffix().str();
    }

    serializePizzaAndMessageQueue(reception, pizzaOrders);
}
