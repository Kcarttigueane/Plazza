/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.hpp
*/

#pragma once

#include "Kitchen.hpp"
#include "PizzaOrder.hpp"
#include "Plazza.hpp"
#include "Process.hpp"

struct KitchenInfo {
    size_t activeOrders;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::unique_ptr<NamedPipeIPC> orderPipe;
    std::unique_ptr<NamedPipeIPC> updatePipe;
};

class Reception {
  private:
    float _timeMultiplier;
    size_t _cookPerKitchen;
    size_t _replenishmentTime;
    size_t _maxOrdersPerKitchen;

    std::vector<pid_t> _kitchenPIDs;
    std::map<pid_t, KitchenInfo> _kitchens;

  public:
    Reception(int multiplier, size_t cooksPerKitchen, size_t replenishmentTime)
        : _timeMultiplier(multiplier),
          _cookPerKitchen(cooksPerKitchen),
          _replenishmentTime(replenishmentTime),
          _maxOrdersPerKitchen(2 * cooksPerKitchen)
    {}

    ~Reception()
    {
        for (const auto& pid : _kitchenPIDs) {
            kill(pid, SIGTERM);
            int status;
            waitpid(pid, &status, 0);
        }
    }

    // ! Getters:

    float getTimeMultiplier() const { return _timeMultiplier; }

    int getCooksPerKitchen() const { return _cookPerKitchen; }

    int getReplenishmentTime() const { return _replenishmentTime; }

    size_t getMaxOrdersPerKitchen() const { return _maxOrdersPerKitchen; }

    NamedPipeIPC& getNamedPipeByPid(pid_t pid)
    {
        auto it = _kitchens.find(pid);

        if (it == _kitchens.end()) {
            throw std::runtime_error("No kitchen with pid " + std::to_string(pid));
        } else {
            return *it->second.orderPipe;
        }
    }

    // ! Methods

    void interactiveShellLoop();
    void sendStatusRequestToAllKitchens();
    void processUpdates();
    void createNewKitchen();
    void closeIdleKitchens();
    void distributeOrder(PizzaOrder& order);
};

void parsePizzaOrder(std::string& input, Reception& reception);
