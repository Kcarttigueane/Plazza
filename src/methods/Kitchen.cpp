/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

// ! Constructor/Destructor:

Kitchen::Kitchen(size_t cooksPerKitchen, size_t replenishmentTime)
    : _cooksPerKitchen(cooksPerKitchen),
      _replenishmentTime(replenishmentTime),
      _running(true),
      _stock(replenishmentTime)
{
    static size_t kitchenId = 0;
    _kitchenId = kitchenId++;
    _replenishmentThread = std::thread([this]() { replenishStock(); });
}

Kitchen::~Kitchen()
{
    _running = false;

    for (auto& cook_thread : _cookThreads) {
        if (cook_thread.joinable()) {
            cook_thread.join();
        }
    }

    if (_replenishmentThread.joinable()) {
        _replenishmentThread.join();
    }
}

// ! Getters:

size_t Kitchen::getKitchenID() const
{
    return _kitchenId;
}

size_t Kitchen::getCooksPerKitchen() const
{
    return _cooksPerKitchen;
}

size_t Kitchen::getReplenishmentTime() const
{
    return _replenishmentTime;
}

std::atomic<bool>& Kitchen::getRunning()
{
    return _running;
}

// ! Methods:

void Kitchen::run()
{
    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThreads.emplace_back([this]() { cook(); });
    }

    while (_running) {
        process_orders();
    }
}

void Kitchen::process_orders() {}

void Kitchen::sendUpdateMessage(const PizzaOrder& order, int _kitchenId)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{}({}/{}) completed: {} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);
}

void Kitchen::cook()
{
    while (_running) {
        PizzaOrder order;
        {
            std::unique_lock<std::mutex> lock(_orderMutex);
            // ! Check if there is any order in the pizza_order_queue
            if (_pizzaOrderQueue.empty()) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            // ! Remove the pizza order from the pizza_order_queue
            order = _pizzaOrderQueue.front();
            _pizzaOrderQueue.pop();
        }

        // ! Cook the pizza and simulate the cooking time
        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<int>(order.getBakingTime() * 1000)));

        sendUpdateMessage(order, _kitchenId);
    }
}

void Kitchen::replenishStock()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::seconds(_replenishmentTime));
        _stock.replenish_stock();
    }
}
