/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.hpp
*/

#pragma once

#include "Ingredients.hpp"
#include "MessageQueueIPC.hpp"
#include "PizzaOrder.hpp"
#include "Plazza.hpp"

class Kitchen {
  private:
    size_t _kitchenId;
    size_t _cooksPerKitchen;
    size_t _replenishmentTime;

    MessageQueueIPC _orderMessageQueue;
    MessageQueueIPC _updatesMessageQueue;

    std::atomic<bool> _running;
    std::vector<std::thread> _cookThreads;
    std::mutex _orderMutex;
    std::queue<PizzaOrder> _pizzaOrderQueue;

    Ingredients _stock;

  public:
    Kitchen(size_t cooks_per_kitchen, size_t replenishment_time,
            const std::string& order_queue_name, const std::string& updates_queue_name);

    ~Kitchen();

    // ! Getters:

    size_t get_kitchen_id() const;
    size_t get_cooks_per_kitchen() const;
    size_t get_replenishment_time() const;
    MessageQueueIPC& get_order_message_queue();
    MessageQueueIPC& get_updates_message_queue();
    std::atomic<bool>& get_running();
    size_t getPizzaOrderQueueLength() const;

    // ! Methods:

    void run();

    void process_orders();

    void sendUpdateMessage(const PizzaOrder& order, int _kitchenId);

    void cook();
};
