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

    for (size_t i = 0; i < totalPizzasOrdered; i++) {
        PizzaType type = getPizzaTypeFromString(std::get<0>(pizzaOrders[i]));
        PizzaSize size = getPizzaSizeFromString(std::get<1>(pizzaOrders[i]));

        size_t pizzaOrderIndex = i + 1;
        PizzaOrder pizzaOrder(orderId, clientId, totalPizzasOrdered, pizzaOrderIndex, type, size,
                              reception.getTimeMultiplier());

        reception.distributeOrder(pizzaOrder);
    }
}

static bool isPizzaOrderValid(string& type, string& size, int number)
{
    auto it_type = PIZZA_TYPES.find(type);
    if (it_type == PIZZA_TYPES.end()) {
        std::cerr << "Error: Invalid pizza type" << std::endl;
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
