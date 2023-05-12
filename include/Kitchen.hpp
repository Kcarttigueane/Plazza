/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Kitchen.hpp
*/

#pragma once

#include "Ingredients.hpp"
#include "NamePipeIPC.hpp"
#include "PizzaOrder.hpp"
#include "Plazza.hpp"

class Kitchen {
  private:
    size_t _kitchenId;
    size_t _cooksPerKitchen;
    size_t _replenishmentTime;

    std::unique_ptr<NamedPipeIPC> _orderPipe;
    std::unique_ptr<NamedPipeIPC> _updatePipe;

    std::atomic<bool> _running;

    std::jthread _replenishmentThread;

    Ingredients _stock;

  public:
    Kitchen(size_t cooksPerKitchen, size_t replenishmentTime, const std::string& orderPipeName,
            const std::string& updatePipeName)
        : _cooksPerKitchen(cooksPerKitchen),
          _replenishmentTime(replenishmentTime),
          _orderPipe(std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Read)),
          _updatePipe(std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Write)),
          _running(true),
          _stock(replenishmentTime)
    {
        _kitchenId++;
    }

    ~Kitchen()
    {
        _running = false;

        // for (auto& cook_thread : _cookThreads) {
        //     if (cook_thread.joinable()) {
        //         cook_thread.join();
        //     }
        // }

        // if (_replenishmentThread.joinable()) {
        //     _replenishmentThread.join();
        // }
    }

    // ! Getters:

    size_t getKitchenID() const { return _kitchenId; }

    size_t getCooksPerKitchen() const { return _cooksPerKitchen; }

    size_t getReplenishmentTime() const { return _replenishmentTime; }

    std::atomic<bool>& getRunning() { return _running; }

    // ! Methods:

    void initThreads();

    void run();

    void sendUpdateMessage(const PizzaOrder& order, int _kitchenId);

    void cook();

    void replenishStock();
};
