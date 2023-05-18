/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.hpp
*/

#pragma once

#include "Kitchen.hpp"
#include "Plazza.hpp"
#include "Process.hpp"

#include <queue>

struct KitchenInfo {
    size_t activeOrders{};
    std::chrono::steady_clock::time_point lastUpdateTime{};
    std::unique_ptr<NamedPipeIPC> orderPipe{};
    std::unique_ptr<NamedPipeIPC> updatePipe{};
    std::deque<size_t> recentLoads{};
    float movingAvgLoad{};
};

class Reception {
  private:
    float _timeMultiplier;
    size_t _cookPerKitchen;
    size_t _replenishmentTime;

    size_t _maxOrdersPerKitchen;
    size_t _loadWindowSize;

    std::vector<pid_t> _kitchenPIDs;
    std::unordered_map<pid_t, KitchenInfo> _kitchens;

    std::mutex _kitchensMutex;

  public:
    Reception(float multiplier, size_t cooksPerKitchen, size_t replenishmentTime,
              size_t loadWindowSize)
        : _timeMultiplier(multiplier),
          _cookPerKitchen(cooksPerKitchen),
          _replenishmentTime(replenishmentTime),
          _maxOrdersPerKitchen(2 * cooksPerKitchen),
          _loadWindowSize(loadWindowSize)
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

    [[nodiscard]] float getTimeMultiplier() const { return _timeMultiplier; }

    [[nodiscard]] size_t getCooksPerKitchen() const { return _cookPerKitchen; }

    [[nodiscard]] size_t getReplenishmentTime() const { return _replenishmentTime; }

    [[nodiscard]] size_t getMaxOrdersPerKitchen() const { return _maxOrdersPerKitchen; }

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
    void processUpdates(std::atomic_bool& running);
    void createNewKitchen();
    void closeIdleKitchens();
    void distributeOrder(PizzaOrder& order);
};

void parsePizzaOrder(std::string& input, Reception& reception);
