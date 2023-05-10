/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

// ! Constructor/Destructor:

Kitchen::Kitchen(size_t cooksPerKitchen, size_t replenishmentTime, const std::string& orderPipeName)
    : _cooksPerKitchen(cooksPerKitchen),
      _replenishmentTime(replenishmentTime),
      _orderPipe(orderPipeName, NamedPipeIPC::Mode::Read),
      //   _updatePipe("update_pipe_" + std::to_string(_kitchenId), NamedPipeIPC::Mode::Write),
      _running(true),
      _stock(replenishmentTime)
{
    _kitchenId++;
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
    std::cout << "Kitchen " << _kitchenId << " is running." << std::endl;
    // for (size_t i = 0; i < _cooksPerKitchen; ++i) {
    //     _cookThreads.emplace_back([this]() { cook(); });
    // }

    while (_running) {
        processOrders();
    }
}

void Kitchen::processOrders()
{
    std::string serialized_order = _orderPipe.read();
    if (!serialized_order.empty()) {
        // PizzaOrder order;
        // std::stringstream ss(serialized_order);
        // ss >> order;

        // std::unique_lock<std::mutex> lock(_orderMutex);
        // _pizzaOrderQueue.push(order);

        std::cout << "serialized_order: " << serialized_order << std::endl;
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order, int _kitchenId)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{}({}/{}) completed: {} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    // _updatePipe.write(msg);
}

void Kitchen::cook()
{
    // while (_running) {
    //     PizzaOrder order;
    //     {
    //         std::unique_lock<std::mutex> lock(_orderMutex);
    //         // ! Check if there is any order in the pizza_order_queue
    //         if (_pizzaOrderQueue.empty()) {
    //             lock.unlock();
    //             std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //             continue;
    //         }
    //         // ! Remove the pizza order from the pizza_order_queue
    //         order = _pizzaOrderQueue.front();
    //         _pizzaOrderQueue.pop();
    //     }

    //     // ! Cook the pizza and simulate the cooking time
    //     std::this_thread::sleep_for(
    //         std::chrono::milliseconds(static_cast<int>(order.getBakingTime() * 1000)));

    //     sendUpdateMessage(order, _kitchenId);
    // }
}

void Kitchen::replenishStock()
{
    // while (_running) {
    //     std::this_thread::sleep_for(std::chrono::seconds(_replenishmentTime));
    //     _stock.replenishStock();
    // }
}
