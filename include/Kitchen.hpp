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

    std::atomic<bool> _running;
    std::vector<std::thread> _cookThreads;
    std::mutex _orderMutex;
    std::queue<PizzaOrder> _pizzaOrderQueue;

    std::thread _replenishmentThread;

    Ingredients _stock;

  public:
    Kitchen(size_t cooks_per_kitchen, size_t replenishment_time);

    ~Kitchen();

    // ! Getters:

    size_t getKitchenID() const;
    size_t getCooksPerKitchen() const;
    size_t getReplenishmentTime() const;
    std::atomic<bool>& getRunning();

    // ! Methods:

    void run();

    void process_orders();

    void sendUpdateMessage(const PizzaOrder& order, int _kitchenId);

    void cook();

    void replenishStock();
};
