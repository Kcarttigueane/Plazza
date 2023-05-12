/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

// ! Methods:

void Kitchen::initThreads()
{
    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThreads.emplace_back(&Kitchen::cook, this);
    }
    _replenishmentThread = std::thread(&Kitchen::replenishStock, this);
}

void Kitchen::run()
{
    // initThreads();

    while (_running) {
        std::string orderStr = _orderPipe->read();
        if (!orderStr.empty()) {

            PizzaOrder order;
            std::istringstream iss(orderStr);
            iss >> order;

            // std::unique_lock<std::mutex> lock(_orderMutex);

            // _pizzaOrderQueue.push(order);

            // lock.unlock();

            std::cout << "Kitchen #" << _kitchenId << " received order: " << orderStr << std::endl;
            sendUpdateMessage(order, _kitchenId);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order, int _kitchenId)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{}({}/{}) completed: {} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    _updatePipe->write(msg);
}

void Kitchen::cook()
{
    // while (_running) {
    //     PizzaOrder order;

    //     {
    //         std::unique_lock<std::mutex> lock(_orderMutex);
    //         if (!_pizzaOrderQueue.empty()) {
    //             order = _pizzaOrderQueue.front();
    //             _pizzaOrderQueue.pop();
    //         } else {
    //             lock.unlock();
    //             std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //             continue;
    //         }
    //     }

    //     if (_stock.checkAndUpdate(order.getPizzaType())) {
    //         // Simulate cooking time
    //         std::this_thread::sleep_for(
    //             std::chrono::milliseconds(static_cast<int>(order.getCookingTime())));

    //         sendUpdateMessage(order, _kitchenId);
    //     }
    // }
}

void Kitchen::replenishStock()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_replenishmentTime));
        _stock.replenishStock();
    }
}
