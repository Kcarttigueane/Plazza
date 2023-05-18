/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

#include <format>
#include <fstream>
#include <iomanip>

void Kitchen::initThreads()
{
    _cookThread.resize(_cooksPerKitchen);
    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThread[i] = std::thread(&Kitchen::cook, this);
    }
    _replenishmentThread = std::thread(&Kitchen::replenishStock, this);
}

void Kitchen::run()
{
    initThreads();

    while (_running) {
        std::string orderStr = _orderPipe->read();
        if (!orderStr.empty()) {
            PizzaOrder order;

            std::istringstream iss(orderStr);
            iss >> order;
            std::map<std::string, int> Ingredients = order.getIngredients();
            std::unique_lock<std::mutex> lock(_orderMutex);
            _pizzaOrderQueue.push_back(order);
            lock.unlock();
            sendUpdateMessage(order);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        if (_cookThread[i].joinable())
            _cookThread[i].join();
    }
    _replenishmentThread.join();
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{} ({}/{}) completed:{} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    std::cout << YELLOW_TEXT(msg) << std::endl;
    _updatePipe->write(msg);
}

void Kitchen::cook()
{
    size_t cookId = IDGenerator::generateID();

    std::cout << "Cook " << cookId << " is ready." << std::endl;

    while (_running) {
        PizzaOrder order;
        std::unique_lock<std::mutex> lock(_orderMutex);
        _orderCV.wait(lock, [this] { return !_pizzaOrderQueue.empty() || !_running; });
        if (!_running)
            break;
        order = _pizzaOrderQueue.front();
        _pizzaOrderQueue.erase(_pizzaOrderQueue.begin());
        std::cout << _pizzaOrderQueue.front() << std::endl;
        lock.unlock();
        std::chrono::milliseconds duration(static_cast<long long>(order.getBakingTime()));
        std::this_thread::sleep_for(duration);
        std::lock_guard<std::mutex> stockLock(_stockMutex);
        std::map<std::string, int> ingredients = order.getIngredients();
        for (const auto& ingredient : ingredients)
            _stock.removeIngredient(ingredient.first, ingredient.second);
        sendUpdateMessage(order);
    }
}

void Kitchen::replenishStock()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_replenishmentTime));

        std::lock_guard<std::mutex> stockLock(_stockMutex);
        _stock.replenishStock();
        std::cout << CYAN_TEXT("Replenished stock") << std::endl;

        _orderCV.notify_all();
    }
}
