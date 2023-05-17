/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.hpp
*/

#pragma once

#include "IDGenerator.hpp"
#include "Ingredients.hpp"
#include "NamedPipeIPC.hpp"
#include "PizzaOrder.hpp"

#include <atomic>
#include <condition_variable>
#include <thread>

class Kitchen {
  private:
    size_t _kitchenId;
    size_t _cooksPerKitchen;
    size_t _replenishmentTime;

    std::unique_ptr<NamedPipeIPC> _orderPipe;
    std::unique_ptr<NamedPipeIPC> _updatePipe;

    std::atomic<bool> _running;

    std::thread _replenishmentThread;
    std::vector<std::thread> _cookThread;

    Ingredients _stock;
    std::mutex _orderMutex;
    std::condition_variable _orderCV;

    std::mutex _stockMutex;

    std::vector<PizzaOrder> _pizzaOrderQueue;
    size_t _timeMultiplier;

  public:
    Kitchen(size_t cooksPerKitchen, size_t replenishmentTime, const std::string& orderPipeName,
            const std::string& updatePipeName, size_t timeMultiplier)
        : _cooksPerKitchen(cooksPerKitchen),
          _replenishmentTime(replenishmentTime),
          _orderPipe(std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Read)),
          _updatePipe(std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Write)),
          _running(true),
          _stock(replenishmentTime),
          _timeMultiplier(timeMultiplier)
    {
        _kitchenId = IDGenerator::generateID();
        _pizzaOrderQueue = std::vector<PizzaOrder>();
    }

    ~Kitchen() { _running = false; }

    // ! Getters:

    [[nodiscard]] size_t getKitchenID() const { return _kitchenId; }

    [[nodiscard]] size_t getCooksPerKitchen() const { return _cooksPerKitchen; }

    [[nodiscard]] size_t getTimeMultiplier() const { return _timeMultiplier; }

    [[nodiscard]] size_t getReplenishmentTime() const { return _replenishmentTime; }

    std::atomic<bool>& getRunning() { return _running; }

    // ! Methods:

    void initThreads();

    void run();

    void sendUpdateMessage(const PizzaOrder& order, int _kitchenId);

    void cook();

    void replenishStock();
};
