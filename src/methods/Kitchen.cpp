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
#include <utility>

void Kitchen::initThreads()
{
    _cookThread.resize(_cooksPerKitchen);

    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThread[i] = std::jthread([this](const std::stop_token& st) { this->cook(st); });
    }

    _replenishmentThread =
        std::jthread([this](const std::stop_token& st) { this->replenishStock(st); });
}

[[noreturn]] void Kitchen::run()
{
    while (true) {
        std::string orderStr = _orderPipe->read();
        if (!orderStr.empty()) {
            size_t pos = orderStr.find(' ');
            std::string first_word = orderStr.substr(0, pos);
            if (first_word == "StatusRequest") {
                sendStatusResponse();
                continue;
            }
            PizzaOrder order;

            std::istringstream iss(orderStr);
            iss >> order;
            {
                std::unique_lock<std::mutex> lock(_orderMutex);
                _pizzaOrderQueue.push_back(order);
                _orderCV.notify_all();
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{} ({}/{}) completed:{} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    // std::cout << YELLOW_TEXT(msg) << std::endl;
    _updatePipe->write(msg);
}

void Kitchen::sendStatusResponse()
{
    std::string statusResponse =
        std::format("StatusResponse: {} {} {} {}", _kitchenId, _cooksPerKitchen,
                    _pizzaOrderQueue.size(), _stock.getTotalStock());

    std::cout << CYAN_TEXT(statusResponse) << std::endl;
    _updatePipe->write(statusResponse);
}

void Kitchen::cook(std::stop_token stopToken)
{
    size_t cookId = _cookId.fetch_add(1);

    std::cout << "Cook " << cookId << " is ready." << std::endl;

    while (!stopToken.stop_requested()) {
        PizzaOrder order;
        {
            std::unique_lock<std::mutex> lock(_orderMutex);
            _orderCV.wait(lock, [this, &stopToken] {
                return !_pizzaOrderQueue.empty() || stopToken.stop_requested();
            });
            if (stopToken.stop_requested())
                break;
            order = _pizzaOrderQueue.front();
            _pizzaOrderQueue.erase(_pizzaOrderQueue.begin());
        }
        std::chrono::seconds duration(static_cast<long long>(order.getBakingTime()));
        std::this_thread::sleep_for(duration);
        {
            std::unique_lock<std::mutex> stockLock(_stockMutex);
            std::map<std::string, int> ingredients = order.getIngredients();
            for (const auto& ingredient : ingredients) {
                _stockCV.wait(stockLock, [&] {
                    return _stock.getStock()[ingredient.first] >= ingredient.second ||
                           stopToken.stop_requested();
                });
                if (stopToken.stop_requested())
                    return;
                _stock.removeIngredient(ingredient.first, ingredient.second);
            }
        }
        sendUpdateMessage(order);
    }
}

void Kitchen::replenishStock(const std::stop_token& st)
{
    while (!st.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_replenishmentTime));
        {
            std::lock_guard<std::mutex> stockLock(_stockMutex);
            for (auto& [ingredient, currentStock] : _stock.getStock()) {
                _stock.addIngredient(ingredient, 1);
            }
        }
        // _stock.printStock();

        _orderCV.notify_all();
        _stockCV.notify_all();
    }
}
