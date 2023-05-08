/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.hpp
*/

#pragma once

#include "Kitchen.hpp"
#include "MessageQueueIPC.hpp"
#include "PizzaOrder.hpp"
#include "Plazza.hpp"
#include "Process.hpp"
#include "UIManager.hpp"

class Reception {
  private:
    float _timeMultiplier;
    size_t _cookPerKitchen;
    size_t _replenishmentTime;

    UIManager& _uiManager;

    // ! Message Queue for sending pizza orders to Kitchen processes

    std::unordered_map<pid_t, MessageQueueIPC> _orderMessageQueues;
    std::unordered_map<pid_t, size_t> _activeOrdersPerKitchen;

    // ! Updates Message Queue for receiving updates from Kitchen processes
    MessageQueueIPC _updatesMessageQueue;

    // ! Add other necessary members for managing updates

    std::mutex _updateMutex;
    std::condition_variable _updatesCV;
    bool _updatesPaused;

    // ! Kitchen processes

    std::vector<pid_t> _kitchenPIDs;
    std::map<pid_t, std::chrono::steady_clock::time_point> _kitchenLastUpdateTimes;

  public:
    Reception(int multiplier, size_t cooks_per_kitchen, size_t replenishment_time,
              UIManager& uiManager);

    virtual ~Reception(){};

    // ! Getters:

    float getTimeMultiplier() const;
    int getCooksPerKitchen() const;
    int getReplenishmentTime() const;
    MessageQueueIPC& getOrderMessageQueueByPid(pid_t pid);
    MessageQueueIPC& getUpdateMessageQueue();
    UIManager& getUIManager();

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
