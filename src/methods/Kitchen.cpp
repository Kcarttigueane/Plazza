/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

#include <fstream>
#include <iomanip>

void Kitchen::initThreads()
{
    _cookThread.resize(_cooksPerKitchen);

    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThread[i] = std::thread([this]() { this->cook(); });
    }

    _replenishmentThread = std::thread([this]() { this->replenishStock(); });
}

void Kitchen::run()
{
    while (!_stopThreads) {
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
    std::ostringstream oss;
    oss << "PizzaOrderResponse: [" << getCurrentTimeString() << "] Order #" << order.getOrderId()
        << " (" << order.getPizzaOrderIndex() << "/" << order.getTotalPizzasOrdered()
        << ") completed : " << order.getTypeString() << " (" << order.getSizeString()
        << ") prepared by Kitchen #" << _kitchenId << "!";

    std::string msg = oss.str();

    // {
    //     std::lock_guard<std::mutex> lock(_printMutex);
    //     std::cout << msg << std::endl;
    // }

    _updatePipe->write(msg);
}

void Kitchen::sendStatusResponse()
{
    std::ostringstream oss;
    oss << "StatusResponse: " << _kitchenId << " " << _cooksPerKitchen << " "
        << _pizzaOrderQueue.size() << " " << _stock.getTotalStock();

    std::string statusResponse = oss.str();

    // {
    //     std::lock_guard<std::mutex> lock(_printMutex);
    //     std::cout << CYAN_TEXT(statusResponse) << std::endl;
    // }

    _updatePipe->write(statusResponse);
}

void Kitchen::cook()
{
    size_t cookId = _cookId.fetch_add(1);

    {
        std::lock_guard<std::mutex> lock(_printMutex);
        std::cout << "Cook " << cookId << " is ready from Kitchen #" << _kitchenId << std::endl;
    }

    while (!_stopThreads) {
        PizzaOrder order;
        {
            std::unique_lock<std::mutex> lock(_orderMutex);
            _orderCV.wait(lock, [this] { return !_pizzaOrderQueue.empty() || _stopThreads; });
            if (_stopThreads)
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
                    return _stock.getStock()[ingredient.first] >= ingredient.second || _stopThreads;
                });
                if (_stopThreads)
                    return;
                _stock.removeIngredient(ingredient.first, ingredient.second);
            }
        }
        sendUpdateMessage(order);
    }
}

void Kitchen::replenishStock()
{
    while (!_stopThreads) {
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
