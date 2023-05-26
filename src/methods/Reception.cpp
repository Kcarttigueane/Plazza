/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.cpp
*/

#include "Reception.hpp"

#include <sys/select.h>
#include <iomanip>
#include <unordered_set>

bool Reception::canWrite()
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);

    struct timeval timeout {};

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int ready = select(STDIN_FILENO + 1, &readSet, nullptr, nullptr, &timeout);
    return (ready > 0 && FD_ISSET(STDIN_FILENO, &readSet));
}

void Reception::interactiveShellLoop()
{
    while (true) {
        if (canWrite()) {
            std::string input;
            std::cout << PROMPT;
            std::getline(std::cin, input);

            if (input == "QUIT")
                break;

            if (input == "status")
                sendStatusRequestToAllKitchens();

            parsePizzaOrder(input, *this);
        }

        closeIdleKitchens();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

    {
        std::lock_guard<std::mutex> lock(_printMutex);
        std::cout << MAGENTA_TEXT("Create New Kitchen #") << MAGENTA_TEXT(newKitchenId)
                  << std::endl;
    }

    pid_t kitchenPid = process.getPid();
    _kitchenPIDs.push_back(kitchenPid);

    KitchenInfo kitchenInfo;
    kitchenInfo.kitchenId = std::make_unique<std::atomic<size_t>>(_kitchenPIDs.size() - 1);
    kitchenInfo.orderPipe =
        std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Write);
    kitchenInfo.updatePipe =
        std::make_unique<NamedPipeIPC>(updatePipeName, NamedPipeIPC::Mode::Read);
    kitchenInfo.activeOrders = 0;
    kitchenInfo.lastUpdateTime = std::chrono::steady_clock::time_point::max();

    std::unique_lock<std::mutex> lock(_kitchensMutex);
    _kitchens[kitchenPid] = std::move(kitchenInfo);
}

void Reception::distributeOrder(PizzaOrder& order)
{
    pid_t targetKitchenPID = FAILURE;
    size_t minActiveOrders = std::numeric_limits<size_t>::max();

    for (const auto& [kitchenPID, kitchenInfo] : _kitchens) {
        if (kitchenInfo.activeOrders < minActiveOrders) {
            minActiveOrders = kitchenInfo.activeOrders;
            targetKitchenPID = kitchenPID;
        }
    }

    if (minActiveOrders >= _maxOrdersPerKitchen) {
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
    targetKitchenInfo.lastUpdateTime = std::chrono::steady_clock::time_point::max();

    if (targetKitchenInfo.recentLoads.size() > _loadWindowSize) {
        size_t removedLoad = targetKitchenInfo.recentLoads.front();
        targetKitchenInfo.recentLoads.pop_front();
        targetKitchenInfo.movingAvgLoad -= removedLoad / static_cast<float>(_loadWindowSize);
    }

    targetKitchenInfo.movingAvgLoad +=
        targetKitchenInfo.activeOrders / static_cast<float>(_loadWindowSize);
}

void Reception::closeIdleKitchens()
{
    const std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::unordered_set<pid_t> idleKitchens;

    for (auto it = _kitchens.begin(); it != _kitchens.end();) {
        const pid_t pid = it->first;
        const std::chrono::steady_clock::time_point lastUpdate = getLastUpdateTime(pid);

        if (lastUpdate == std::chrono::steady_clock::time_point::max() ||
            it->second.activeOrders > 0) {
            ++it;
            continue;
        }

        const std::chrono::duration<double> elapsedSeconds = currentTime - lastUpdate;
        if (elapsedSeconds.count() > 5.0) {
            idleKitchens.insert(pid);
            ++it;
        } else
            ++it;
    }

    for (const pid_t pid : idleKitchens) {
        size_t kitchenIdKilled = getKitchenIdByPid(pid);
        {
            std::lock_guard<std::mutex> lock(_printMutex);
            std::cout << "Kitchen " << kitchenIdKilled << " has been closed." << std::endl;
        }
        kill(pid, SIGTERM);
        int status;
        waitpid(pid, &status, 0);
        _kitchens.erase(pid);
        _kitchenPIDs.erase(std::remove(_kitchenPIDs.begin(), _kitchenPIDs.end(), pid),
                           _kitchenPIDs.end());
    }
}

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
        std::cout << "'status': No kitchens are currently running." << std::endl;
        return;
    }

    if (_statusResponses.size() != _kitchenPIDs.size()) {
        std::cout << "'status': Not all kitchens have responded yet to the statusRequest"
                  << std::endl;
        return;
    }

    std::cout << std::setw(10) << "Kitchen ID"
              << " | " << std::setw(18) << "Pizzas in Progress"
              << " | " << std::setw(15) << "Available Cooks"
              << " | " << std::setw(16) << "Ingredient Stock" << std::endl;

    for (const auto& statusResponse : _statusResponses) {
        std::cout << statusResponse << '\n';
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
                // std::cout << MAGENTA_TEXT(ingredientStock) << std::endl;
                // std::string formattedString =
                //     std::format("{0:<10} | {1:<18} | {2:<15} | {3:<16}", kitchenID,
                //                 pizzasInProgress, availableCooks, ingredientStock);
                // _statusResponses.push_back(formattedString);

                std::ostringstream formattedStream;
                formattedStream << std::setw(10) << kitchenID << " | " << std::setw(18)
                                << pizzasInProgress << " | " << std::setw(15) << availableCooks
                                << " | " << std::setw(16) << ingredientStock;
                std::string formattedString = formattedStream.str();

                _statusResponses.push_back(formattedString);

                {
                    displayStatusResponses();
                }
            } else {
                {
                    std::lock_guard<std::mutex> lock(_printMutex);
                    std::cout << MAGENTA_TEXT(update) << std::endl;
                }
                appendToFile("log.txt", update);
                _kitchens[pid].activeOrders--;
                _kitchens[pid].lastUpdateTime = std::chrono::steady_clock::now();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
