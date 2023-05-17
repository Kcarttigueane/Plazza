/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** PizzaOrder.cpp
*/

#include "PizzaOrder.hpp"

// ! Constructors:

PizzaOrder::PizzaOrder(size_t orderId, size_t clientId, size_t totalPizzasOrdered,
                       size_t pizzaOrderIndex, PizzaType type, PizzaSize size, float timeMultiplier)
    : _orderId(orderId),
      _clientId(clientId),
      _totalPizzasOrdered(totalPizzasOrdered),
      _pizzaOrderIndex(pizzaOrderIndex),
      _type(type),
      _size(size),
      _timeMultiplier(timeMultiplier)
{}

// ! Getters:

size_t PizzaOrder::getOrderId() const
{
    return _orderId;
}

size_t PizzaOrder::getClientId() const
{
    return _clientId;
}

size_t PizzaOrder::getTotalPizzasOrdered() const
{
    return _totalPizzasOrdered;
}

size_t PizzaOrder::getPizzaOrderIndex() const
{
    return _pizzaOrderIndex;
}

PizzaType PizzaOrder::getType() const
{
    return _type;
}

size_t PizzaOrder::getTimeMultiplier() const
{
    return _timeMultiplier;
}

std::string PizzaOrder::getTypeString() const
{
    switch (_type) {
        case Regina:
            return "Regina";
        case Margarita:
            return "Margarita";
        case Americana:
            return "Americana";
        case Fantasia:
            return "Fantasia";
    }
    return "";
}

PizzaSize PizzaOrder::getSize() const
{
    return _size;
}

std::string PizzaOrder::getSizeString() const
{
    switch (_size) {
        case S:
            return "S";
        case M:
            return "M";
        case L:
            return "L";
        case XL:
            return "XL";
        case XXL:
            return "XXL";
    }
    return "";
}

int PizzaOrder::getBakingTime() const
{
    return _bakingTime;
}

// ! Setters:

void PizzaOrder::setIngredients()
{
    _ingredients["doe"] = 1;
    _ingredients["tomato"] = 1;
    switch (_type) {
        case Regina:
            _ingredients["gruyere"] = 1;
            _ingredients["ham"] = 1;
            _ingredients["mushrooms"] = 1;
            break;
        case Margarita:
            _ingredients["gruyere"] = 1;
            break;
        case Americana:
            _ingredients["gruyere"] = 1;
            _ingredients["steak"] = 1;
            break;
        case Fantasia:
            _ingredients["eggplant"] = 1;
            _ingredients["goat_cheese"] = 1;
            _ingredients["chief_love"] = 1;
            break;
    }
}

void PizzaOrder::setBakingTime()
{
    switch (_type) {
        case Regina:
            _bakingTime = 2 * _timeMultiplier;
            break;
        case Margarita:
            _bakingTime = 1 * _timeMultiplier;
            break;
        case Americana:
            _bakingTime = 2 * _timeMultiplier;
            break;
        case Fantasia:
            _bakingTime = 4 * _timeMultiplier;
            break;
    }
}

std::map<std::string,int> PizzaOrder::getIngredients()
{
    return _ingredients;
}

// ! Overload operator<<:

std::ostream& operator<<(std::ostream& os, const PizzaOrder& order)
{
    os << static_cast<int>(PizzaOrderRequest) << " " << order._orderId << " " << order._clientId
       << " " << order._totalPizzasOrdered << " " << order._pizzaOrderIndex << " "
       << static_cast<int>(order._type) << " " << static_cast<int>(order._size) << " " << order._timeMultiplier;
    return os;
}

// ! Overload operator>>:

std::istream& operator>>(std::istream& is, PizzaOrder& order)
{
    int messageType, type, size;

    is >> messageType >> order._orderId >> order._clientId >> order._totalPizzasOrdered >>
        order._pizzaOrderIndex >> type >> size >> order._timeMultiplier;
    if (static_cast<MessageType>(messageType) == PizzaOrderRequest) {
        order._type = static_cast<PizzaType>(type);
        order._size = static_cast<PizzaSize>(size);

        order.setIngredients();
        order.setBakingTime();
    }

    return is;
}
