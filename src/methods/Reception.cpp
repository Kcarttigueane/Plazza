/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.cpp
*/

#include "Reception.hpp"

#include <format>
#include <iomanip>

void Reception::interactiveShellLoop()
{
    while (true) {
        std::string input;
        std::cout << PROMPT;
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
    std::string orderPipeName = "NamedPipes/orderPipe_" + std::to_string(_kitchenPIDs.size());
    std::string updatePipeName = "NamedPipes/updatePipe_" + std::to_string(_kitchenPIDs.size());

    size_t newKitchenId = _kitchenPIDs.size();

    Process process([&, orderPipeName, updatePipeName]() {
        Kitchen kitchen(_cookPerKitchen, _replenishmentTime, orderPipeName, updatePipeName,
                        _timeMultiplier, newKitchenId);
        kitchen.run();
    });

    pid_t kitchenPid = process.getPid();
    _kitchenPIDs.push_back(kitchenPid);

    KitchenInfo kitchenInfo;
    kitchenInfo.kitchenId = std::make_unique<std::atomic<size_t>>(_kitchenPIDs.size() - 1);
    kitchenInfo.orderPipe =
        std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Write);
    kitchenInfo.updatePipe =
        std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Read);

    std::unique_lock<std::mutex> lock(_kitchensMutex);
    _kitchens[kitchenPid] = std::move(kitchenInfo);
}

void Reception::distributeOrder(PizzaOrder& order)
{
    pid_t targetKitchenPID = FAILURE;
    size_t minActiveOrders = std::numeric_limits<size_t>::max();

    // Find the kitchen with the least active orders
    for (const auto& [kitchenPID, kitchenInfo] : _kitchens) {
        if (kitchenInfo.activeOrders < minActiveOrders) {
            minActiveOrders = kitchenInfo.activeOrders;
            targetKitchenPID = kitchenPID;
        }
    }

    // If all kitchens are fully loaded (or there are no kitchens), create a new one
    if (minActiveOrders >= _maxOrdersPerKitchen) {
        createNewKitchen();
        targetKitchenPID = _kitchenPIDs.back();
    }

    // Send the order to the selected kitchen
    NamedPipeIPC& orderPipe = getNamedPipeByPid(targetKitchenPID);
    std::ostringstream oss;
    oss << order;
    std::string serializedPizzaOrder = oss.str();
    orderPipe.write(serializedPizzaOrder);

    // Update the load of the kitchen
    KitchenInfo& targetKitchenInfo = _kitchens[targetKitchenPID];
    targetKitchenInfo.activeOrders++;
    targetKitchenInfo.recentLoads.push_back(targetKitchenInfo.activeOrders);

    if (targetKitchenInfo.recentLoads.size() > _loadWindowSize) {
        size_t removedLoad = targetKitchenInfo.recentLoads.front();
        targetKitchenInfo.recentLoads.pop_front();
        targetKitchenInfo.movingAvgLoad -= removedLoad / static_cast<float>(_loadWindowSize);
    }

    targetKitchenInfo.movingAvgLoad +=
        targetKitchenInfo.activeOrders / static_cast<float>(_loadWindowSize);
}

void Reception::closeIdleKitchens() {}

void Reception::sendStatusRequestToAllKitchens()
{
    for (auto& namedPipeEntry : _kitchens) {
        namedPipeEntry.second.orderPipe->write("StatusRequest");
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

void Reception::displayStatusResponses()
{
    if (_kitchens.empty()) {
        std::cout << "No kitchens are currently running." << std::endl;
        return;
    }

    if (_statusResponses.size() != _kitchenPIDs.size()) {
        std::cout << "Not all kitchens have responded yet." << std::endl;
        return;
    }

    std::cout << std::setw(10) << "Kitchen ID"
              << " | " << std::setw(18) << "Pizzas in Progress"
              << " | " << std::setw(15) << "Available Cooks"
              << " | " << std::setw(16) << "Ingredient Stock" << std::endl;

    for (const auto& statusResponse : _statusResponses) {
        std::cout << statusResponse << std::endl;
    }

    _statusResponses.clear();
}

void Reception::processUpdates(std::atomic_bool& stopThread)
{
    while (!stopThread) {
        for (const auto& [pid, kitchenInfo] : _kitchens) {
            std::unique_lock<std::mutex> lock(_kitchensMutex);

            NamedPipeIPC& updatePipe = *kitchenInfo.updatePipe;

            std::string update = updatePipe.read();

            if (update.empty()) {
                continue;
            }

            size_t pos = update.find(':');
            std::string first_word = update.substr(0, pos);

            if (first_word == "StatusResponse") {
                auto statusTuple = parseStatusResponse(update);
                std::string kitchenID = std::get<0>(statusTuple);
                int availableCooks = std::get<1>(statusTuple);
                int pizzasInProgress = std::get<2>(statusTuple);
                std::string ingredientStock = std::get<3>(statusTuple);
                std::cout << MAGENTA_TEXT(ingredientStock) << std::endl;
                std::string formattedString =
                    std::format("{0:<10} | {1:<18} | {2:<15} | {3:<16}", kitchenID,
                                pizzasInProgress, availableCooks, ingredientStock);
                _statusResponses.push_back(formattedString);
                displayStatusResponses();
            } else {
                std::cout << MAGENTA_TEXT(update) << std::endl;
                appendToFile("log.txt", update);
                _kitchens[pid].activeOrders--;
                _kitchens[pid].lastUpdateTime = std::chrono::steady_clock::now();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
