/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Reception.cpp
*/

#include "Reception.hpp"

// ! Methods:

void Reception::interactive_shell_loop()
{
    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "QUIT") {
            break;
        }

        if (input == "status") {
            send_status_request_to_all_kitchens();
        }

        parse_pizza_order(input, *this);
    }
}

void Reception::create_new_kitchen()
{
    std::string orderPipeName = "/tmp/order_pipe_0";

    pid_t kitchenPid = fork();

    if (kitchenPid < 0) {
        std::cerr << "Error creating kitchen process." << std::endl;
        exit(1);
    }

    if (kitchenPid == 0) {  // ! Child process
        Kitchen kitchen(_cookPerKitchen, _replenishmentTime, orderPipeName);
        sleep(1);

        kitchen.run();
    } else {  // ! Parent process
        std::cout << "pid: " << kitchenPid << std::endl;
        _kitchenPIDs.push_back(kitchenPid);
        _kitchenPipes[kitchenPid] =
            std::make_unique<NamedPipeIPC>(orderPipeName, NamedPipeIPC::Mode::Write);
    }
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
    std::ostringstream oss;
    oss << order;
    std::string serializedPizzaOrder = oss.str();

    getNamedPipeByPid(selectedPID).write(serializedPizzaOrder);
}

void Reception::manage_kitchens() {}

void Reception::send_status_request_to_all_kitchens()
{
    for (auto& namedPipeEntry : _kitchenPipes) {
        namedPipeEntry.second->write("statusRequest");
    }
}

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
    } else {
        std::cout << update << std::endl;
        // appendToFile("log.txt", update);
    }
}
