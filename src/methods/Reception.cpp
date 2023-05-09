/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.cpp
*/

#include "Reception.hpp"

Reception::Reception(int multiplier, size_t cooks_per_kitchen, size_t replenishment_time,
                     UIManager& uiManager)
    : _timeMultiplier(multiplier),
      _cookPerKitchen(cooks_per_kitchen),
      _replenishmentTime(replenishment_time),
      _uiManager(uiManager),
      _updatesMessageQueue("/UpdatesMessageQueue"),
      _updatesPaused(false)
{}

// ! Getters:

float Reception::getTimeMultiplier() const
{
    return _timeMultiplier;
}

int Reception::getCooksPerKitchen() const
{
    return _cookPerKitchen;
}

int Reception::getReplenishmentTime() const
{
    return _replenishmentTime;
}

MessageQueueIPC& Reception::getOrderMessageQueueByPid(pid_t pid)
{
    auto it = _orderMessageQueues.find(pid);

    if (it == _orderMessageQueues.end()) {
        throw std::runtime_error("No order message queue found for PID " + std::to_string(pid));
    } else {
        return it->second;
    }
}

MessageQueueIPC& Reception::getUpdateMessageQueue()
{
    return _updatesMessageQueue;
}

UIManager& Reception::getUIManager()
{
    return _uiManager;
}

// ! Methods:

std::tuple<std::string, int, int, std::string> parse_status_response(
    const std::string& statusResponse)
{
    std::istringstream ss(statusResponse);
    std::string prefix;
    std::string kitchenID;
    int cooksPerKitchen;
    int pizzaOrderQueueLength;
    std::string stock;

    ss >> prefix >> kitchenID >> cooksPerKitchen >> pizzaOrderQueueLength >> stock;

    return std::make_tuple(kitchenID, cooksPerKitchen, pizzaOrderQueueLength, stock);
}

void display_status_response(const std::string& kitchenID, int pizzasInProgress, int availableCooks,
                             const std::string& ingredientStock)
{
    std::cout << "status" << std::endl;
    std::cout << std::setw(10) << "Kitchen ID"
              << " | " << std::setw(18) << "Pizzas in Progress"
              << " | " << std::setw(15) << "Available Cooks"
              << " | " << std::setw(16) << "Ingredient Stock" << std::endl;
    std::cout << std::setw(10) << kitchenID << " | " << std::setw(18) << pizzasInProgress << " | "
              << std::setw(15) << availableCooks << " | " << std::setw(16) << ingredientStock
              << std::endl;
}

void Reception::interactive_shell_loop()
{
    while (true) {
        std::string input = _uiManager.get_user_input();

        if (input == "QUIT") {
            break;
        }

        if (input == "status") {
            send_status_request_to_all_kitchens();
        }

        parse_pizza_order(input, *this);
    }
}

void Reception::send_status_request_to_all_kitchens()
{
    for (auto& orderMessageQueueEntry : _orderMessageQueues) {
        orderMessageQueueEntry.second.send("statusRequest");
    }
}

void Reception::process_updates()
{
    // ? Get the update string from the message queue or other source of updates : while true ?
    std::string update =
        "PizzaOrderResponse: #124 (1/7) completed: Margarita(L) prepared by Kitchen #2";

    size_t pos = update.find(":");
    std::string first_word = update.substr(0, pos);

    if (first_word == "statusResponse") {
        auto statusTuple = parse_status_response(update);
        std::string kitchenID = std::get<0>(statusTuple);
        int availableCooks = std::get<1>(statusTuple);
        int pizzasInProgress = std::get<2>(statusTuple);
        std::string ingredientStock = std::get<3>(statusTuple);
        display_status_response(kitchenID, pizzasInProgress, availableCooks, ingredientStock);
        _uiManager.display_update(update);
    } else {
        _uiManager.display_update(update);
        appendToFile("log.txt", update);
    }
}

void Reception::create_new_kitchen()
{
    // Process process([this]() {
    //     Kitchen kitchen(_cookPerKitchen, _replenishmentTime,
    //                     "/OrderMessageQueue_" + std::to_string(getpid()), "/UpdatesMessageQueue");
    //     kitchen.run();
    // });

    // pid_t new_pid = process.getPid();
    // _kitchenPIDs.push_back(new_pid);

    // // ! Create a new Order Message Queue for the new kitchen process
    // _orderMessageQueues[new_pid] =
    //     MessageQueueIPC("/OrderMessageQueue_" + std::to_string(new_pid), true);
}

void Reception::close_idle_kitchens() {}

// ? this function logic should change for load balancing
void Reception::distribute_order(PizzaOrder& order)
{
    // ! If there are no kitchens, create one:
    if (_kitchenPIDs.empty()) {
        create_new_kitchen();
    }

    // ! Find the kitchen with the least number of active orders:
    pid_t selectedPID = _kitchenPIDs.front();
    size_t minActiveOrders = _activeOrdersPerKitchen[selectedPID];

    for (const pid_t& pid : _kitchenPIDs) {
        if (_activeOrdersPerKitchen[pid] < minActiveOrders) {
            selectedPID = pid;
            minActiveOrders = _activeOrdersPerKitchen[pid];
        }
    }

    // ! Increment the active orders count for the selected kitchen
    _activeOrdersPerKitchen[selectedPID]++;

    // ! Serialize the pizza order and send it to the selected kitchen's message queue
    // std::ostringstream oss;
    // oss << pizzaOrder;
    // string serializedPizzaOrder = oss.str();

    std::stringstream serialized_order;
    serialized_order << order;

    getOrderMessageQueueByPid(selectedPID).send(serialized_order.str());
}

void Reception::manage_kitchens() {}
