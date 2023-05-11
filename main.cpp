/*
** EPITECH PROJECT, 2023
** B-CCP-400-LYN-4-1-theplazza-jules.dutel
** File description:
** main.c
*/

#include "./include/IDGenerator.hpp"
#include "./include/Plazza.hpp"
#include "./include/Process.hpp"
#include "./include/Reception.hpp"

size_t IDGenerator::currentID = 0;

int main(int argc, char* argv[])
{
    float timeMultiplier;
    size_t cooksPerKitchen, replenishmentTime;

    try {
        get_program_arguments(argc, argv, timeMultiplier, cooksPerKitchen, replenishmentTime);

        Reception reception(timeMultiplier, cooksPerKitchen, replenishmentTime);

        std::thread updatesDisplayThread([&reception]() { reception.processUpdates(); });

        reception.interactiveShellLoop();

        updatesDisplayThread.join();

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return ERROR;
    }

    return EXIT_SUCCESS;
}
