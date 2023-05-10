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
#include "UIManager.hpp"

class Reception {
  private:
    float _timeMultiplier;
    size_t _cookPerKitchen;
    size_t _replenishmentTime;

    // UIManager& _uiManager;

    std::vector<pid_t> _kitchenPIDs;

    std::map<pid_t, std::chrono::steady_clock::time_point> _kitchenLastUpdateTimes;

    std::unordered_map<pid_t, std::unique_ptr<NamedPipeIPC>> _orderPipes;
    std::map<pid_t, NamedPipeIPC> _UpdatesPipes;

    std::map<pid_t, size_t> _activeOrdersPerKitchen;

  public:
    Reception(int multiplier, size_t cooks_per_kitchen, size_t replenishment_time)
        : _timeMultiplier(multiplier),
          _cookPerKitchen(cooks_per_kitchen),
          _replenishmentTime(replenishment_time)
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

    NamedPipeIPC& getNamedPipeByPid(pid_t pid)
    {
        auto it = _orderPipes.find(pid);

        if (it == _orderPipes.end()) {
            throw std::runtime_error("No order message queue found for PID " + std::to_string(pid));
        } else {
            return *it->second;
        }
    }

    // ! Methods

    void interactive_shell_loop();
    void send_status_request_to_all_kitchens();
    void process_updates();
    void create_new_kitchen();
    void close_idle_kitchens();
    void distribute_order(PizzaOrder& order);
    void manage_kitchens();
};

void parse_pizza_order(std::string& input, Reception& reception);
