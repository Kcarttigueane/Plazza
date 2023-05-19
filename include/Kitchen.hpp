/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.hpp
*/

#pragma once

#include "Ingredients.hpp"
#include "NamedPipeIPC.hpp"
#include "PizzaOrder.hpp"

#include <atomic>
#include <condition_variable>
#include <stop_token>
#include <thread>
#include <vector>

class Kitchen {
  private:
    size_t _kitchenId;
    size_t _cooksPerKitchen;
    size_t _replenishmentTime;
    float _timeMultiplier;

    std::unique_ptr<NamedPipeIPC> _orderPipe;
    std::unique_ptr<NamedPipeIPC> _updatePipe;

    std::jthread _replenishmentThread;
    std::vector<std::jthread> _cookThread;

    Ingredients _stock;
    std::vector<PizzaOrder> _pizzaOrderQueue;

    std::mutex _orderMutex;
    std::condition_variable _orderCV;
    std::condition_variable _stockCV;

    std::mutex _stockMutex;

    std::atomic<size_t> _cookId;

  public:
    Kitchen(size_t cooksPerKitchen, size_t replenishmentTime, const std::string& orderPipeName,
            const std::string& updatePipeName, float timeMultiplier, size_t kitchenId)
        : _kitchenId(kitchenId),
          _cooksPerKitchen(cooksPerKitchen),
          _replenishmentTime(replenishmentTime),
          _timeMultiplier(timeMultiplier),
          _orderPipe(std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Read)),
          _updatePipe(std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Write)),
          _stock(replenishmentTime),
          _cookId(0)
    {
        _pizzaOrderQueue = std::vector<PizzaOrder>();
        initThreads();
    }

    ~Kitchen() = default;

    // ! Getters:

    [[nodiscard]] size_t getKitchenID() const { return _kitchenId; }

    [[nodiscard]] size_t getCooksPerKitchen() const { return _cooksPerKitchen; }

    [[nodiscard]] float getTimeMultiplier() const { return _timeMultiplier; }

    [[nodiscard]] size_t getReplenishmentTime() const { return _replenishmentTime; }

    // ! Methods:

    void initThreads();

    [[noreturn]] void run();

    void sendUpdateMessage(const PizzaOrder& order);

    void sendStatusResponse();

    void cook(std::stop_token stopToken);

    void replenishStock(const std::stop_token& st);
};
