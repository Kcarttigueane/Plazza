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
    std::string orderPipeName = "orderPipe_" + std::to_string(_kitchenPIDs.size());
    std::string updatePipeName = "updatePipe_" + std::to_string(_kitchenPIDs.size());

    Process process([&, orderPipeName, updatePipeName]() {  // ! Child process
        Kitchen kitchen(_cookPerKitchen, _replenishmentTime, orderPipeName, updatePipeName);
        kitchen.run();
    });

    pid_t kitchenPid = process.getPid();
    _kitchenPIDs.push_back(kitchenPid);

    KitchenInfo kitchenInfo;
    kitchenInfo.activeOrders = 0;
    kitchenInfo.orderPipe =
        std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Write);
    kitchenInfo.updatePipe =
        std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Read);

    std::unique_lock<std::mutex> lock(_kitchensMutex);
    _kitchens[kitchenPid] = std::move(kitchenInfo);
}

void Reception::distributeOrder(PizzaOrder& order)
{
    pid_t targetKitchenPID = -1;
    float minLoad = std::numeric_limits<float>::max();

    for (const auto& [kitchenPID, kitchenInfo] : _kitchens) {
        if (kitchenInfo.activeOrders < _maxOrdersPerKitchen) {
            float load = kitchenInfo.movingAvgLoad;
            if (load < minLoad) {
                minLoad = load;
                targetKitchenPID = kitchenPID;
            }
        }
    }

    if (targetKitchenPID == -1) {
        createNewKitchen();
        targetKitchenPID = _kitchenPIDs.back();
    }

    NamedPipeIPC& orderPipe = getNamedPipeByPid(targetKitchenPID);
    std::ostringstream oss;
    oss << order;
    std::string serializedPizzaOrder = oss.str();

    orderPipe.write(serializedPizzaOrder);

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
        namedPipeEntry.second.orderPipe->write("statusRequest");
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

            size_t pos = update.find(":");
            std::string first_word = update.substr(0, pos);

            if (first_word == "statusResponse") {
                auto statusTuple = parseStatusResponse(update);
                std::string kitchenID = std::get<0>(statusTuple);
                int availableCooks = std::get<1>(statusTuple);
                int pizzasInProgress = std::get<2>(statusTuple);
                // displayStatusResponse(kitchenID, pizzasInProgress, availableCooks, ingredientStock);
            } else {
                std::cout << update << std::endl;
                appendToFile("log.txt", update);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Add a short sleep duration
    }
}
