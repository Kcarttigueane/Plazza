/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

// void Kitchen::initThreads()
// {

//     _cookThread.resize(_cooksPerKitchen);
//     for (size_t i = 0; i < _cooksPerKitchen; ++i) {
//         _cookThread[i] = std::thread(&Kitchen::cook, this);
//     }
//     // _replenishmentThread = std::thread(&Kitchen::replenishStock, this);
// }

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
            std::cout << "Order pushed to queue: " << order << std::endl;

            lock.unlock();
            sendUpdateMessage(order, _kitchenId);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        }
    }

    // for (auto& cookThread : _cookThread) {
    //     if (cookThread.joinable()) {
    //         cookThread.join();
    //     }
    // }
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order, int _kitchenId)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{} ({}/{}) completed:{} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    std::cout << YELLOW_TEXT(msg) << std::endl;
    _updatePipe->write(msg);
}

// void Kitchen::cook()
// {
//     std::cout << CYAN_TEXT("Cooking thread is running") << std::endl;
//     while (_running) {
//         PizzaOrder order;
//         std::unique_lock<std::mutex> lock(_orderMutex);
//         if (!_pizzaOrderQueue.empty()) {
//             std::cout << _pizzaOrderQueue.size() << std::endl;
//             order = _pizzaOrderQueue.front();
//             std::cout << order << std::endl;
//             _pizzaOrderQueue.erase(_pizzaOrderQueue.begin());
//             std::cout << _pizzaOrderQueue.size() << std::endl;
//             lock.unlock();
//         } else {
//             lock.unlock();
//             std::this_threadz::sleep_for(std::chrono::milliseconds(100));
//             continue;
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(order.getBakingTime()));


//         sendUpdateMessage(order, _kitchenId);
//     }
// }

void Kitchen::replenishStock()
{
    while (_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(_replenishmentTime));
        _stock.replenishStock();
    }
}
