/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.cpp
*/

#include "Reception.hpp"

// ! Methods:

void Reception::interactiveShellLoop()
{
    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "QUIT") {
            break;
        }

        if (input == "status") {
            sendStatusRequestToAllKitchens();
        }

        parsePizzaOrder(input, *this);
    }
}

void Reception::createNewKitchen()
{
    std::string orderPipeName = "/tmp/order_pipe_" + std::to_string(_kitchenPIDs.size());
    std::string updatePipeName = "/tmp/update_pipe_" + std::to_string(_kitchenPIDs.size());

    Process kitchenProcess([this, &orderPipeName] {
        Kitchen kitchen(_cookPerKitchen, _replenishmentTime, orderPipeName);
        sleep(1);
        kitchen.run();
    });

    pid_t kitchenPid = kitchenProcess.getPid();

    std::cout << "pid: " << kitchenPid << std::endl;
    _kitchenPIDs.push_back(kitchenPid);
    KitchenInfo kitchenInfo;
    kitchenInfo.activeOrders = 0;
    kitchenInfo.lastUpdateTime = std::chrono::steady_clock::now();
    kitchenInfo.orderPipe =
        std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Write);
    // kitchenInfo.updatePipe = NamedPipeIPC(updatePipeName, NamedPipeIPC::Mode::Read);
    _kitchens[kitchenPid] = std::move(kitchenInfo);
}

void Reception::distributeOrder(PizzaOrder& order)
{
    // ! If there are no kitchens, create one:
    if (_kitchenPIDs.empty()) {
        createNewKitchen();
    }

    // ! Find the kitchen with the least number of active orders:
    pid_t selectedPID = _kitchenPIDs.front();
    size_t minActiveOrders = _kitchens[selectedPID].activeOrders;

    for (const pid_t& pid : _kitchenPIDs) {
        if (_kitchens[pid].activeOrders < minActiveOrders) {
            selectedPID = pid;
            minActiveOrders = _kitchens[pid].activeOrders;
        }
    }

    // ! If the selected kitchen has reached the maximum allowed active orders, create a new one:
    if (minActiveOrders >= _maxOrdersPerKitchen) {
        createNewKitchen();
        selectedPID = _kitchenPIDs.back();
    }

    // ! Increment the active orders count for the selected kitchen
    _kitchens[selectedPID].activeOrders++;

    // ! Serialize the pizza order and send it to the selected kitchen's order pipe
    std::ostringstream oss;
    oss << order;
    std::string serializedPizzaOrder = oss.str();

    _kitchens[selectedPID].orderPipe->write(serializedPizzaOrder);
}

void Reception::closeIdleKitchens() {}

void Reception::sendStatusRequestToAllKitchens()
{
    for (auto& namedPipeEntry : _kitchens) {
        namedPipeEntry.second.orderPipe->write("status");
    }
}

std::tuple<std::string, int, int, std::string> parseStatusResponse(
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

void displayStatusResponse(const std::string& kitchenID, int pizzasInProgress, int availableCooks,
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

void Reception::processUpdates()
{
    // ? Get the update string from the message queue or other source of updates : while true ?
    std::string update =
        "PizzaOrderResponse: #124 (1/7) completed: Margarita(L) prepared by Kitchen #2";

    size_t pos = update.find(":");
    std::string first_word = update.substr(0, pos);

    if (first_word == "statusResponse") {
        auto statusTuple = parseStatusResponse(update);
        std::string kitchenID = std::get<0>(statusTuple);
        int availableCooks = std::get<1>(statusTuple);
        int pizzasInProgress = std::get<2>(statusTuple);
        std::string ingredientStock = std::get<3>(statusTuple);
        displayStatusResponse(kitchenID, pizzasInProgress, availableCooks, ingredientStock);
    } else {
        std::cout << update << std::endl;
        // appendToFile("log.txt", update);
    }
}
