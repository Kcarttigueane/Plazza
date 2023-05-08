/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.cpp
*/

#include "Kitchen.hpp"

Kitchen::Kitchen(size_t cooksPerKitchen, size_t replenishmentTime,
                 const std::string& orderMessageQueueName,
                 const std::string& updatesMessageQueueName)
    : _cooksPerKitchen(cooksPerKitchen),
      _replenishmentTime(replenishmentTime),
      _orderMessageQueue(orderMessageQueueName, true),
      _updatesMessageQueue(updatesMessageQueueName, false),
      _running(true),
      _stock(replenishmentTime)
{
    static size_t kitchenId = 0;
    _kitchenId = kitchenId++;
}

Kitchen::~Kitchen()
{
    _running = false;

    for (auto& cook_thread : _cookThreads) {
        if (cook_thread.joinable()) {
            cook_thread.join();
        }
    }
}

size_t Kitchen::get_kitchen_id() const
{
    return _kitchenId;
}

size_t Kitchen::get_cooks_per_kitchen() const
{
    return _cooksPerKitchen;
}

size_t Kitchen::get_replenishment_time() const
{
    return _replenishmentTime;
}

MessageQueueIPC& Kitchen::get_order_message_queue()
{
    return _orderMessageQueue;
}

MessageQueueIPC& Kitchen::get_updates_message_queue()
{
    return _updatesMessageQueue;
}

std::atomic<bool>& Kitchen::get_running()
{
    return _running;
}

size_t Kitchen::getPizzaOrderQueueLength() const
{
    return _pizzaOrderQueue.size();
}

void Kitchen::run()
{
    for (size_t i = 0; i < _cooksPerKitchen; ++i) {
        _cookThreads.emplace_back([this]() { cook(); });
    }

    while (_running) {
        process_orders();
    }
}

void Kitchen::process_orders()
{
    std::string serialized_order;

    try {
        serialized_order = _orderMessageQueue.receive();

        if (serialized_order == "StatusRequest") {
            std::stringstream ss;
            ss << "StatusResponse: " << _kitchenId << " " << _cooksPerKitchen << " "
               << getPizzaOrderQueueLength() << " " << _stock.getIndividualStock();

            _updatesMessageQueue.send(ss.str());
            return;
        }

        PizzaOrder order;
        std::stringstream ss(serialized_order);
        ss >> order;

        std::unique_lock<std::mutex> lock(_orderMutex);

        _pizzaOrderQueue.push(order);

        lock.unlock();

    } catch (const std::runtime_error& e) {
        // ! Handle the exception, e.g. log it or ignore it
    }
}

void Kitchen::sendUpdateMessage(const PizzaOrder& order, int _kitchenId)
{
    std::string msg = std::format(
        "PizzaOrderResponse: [{}] Order #{}({}/{}) completed: {} ({}) prepared by Kitchen #{}!",
        getCurrentTimeString(), order.getOrderId(), order.getPizzaOrderIndex(),
        order.getTotalPizzasOrdered(), order.getTypeString(), order.getSizeString(), _kitchenId);

    _updatesMessageQueue.send(msg);
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
