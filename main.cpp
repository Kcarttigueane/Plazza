/*
** EPITECH PROJECT, 2023
** B-CCP-400-LYN-4-1-theplazza-jules.dutel
** File description:
** main.c
*/

#include "./include/Plazza.hpp"

int main(int argc, char* argv[])
{
    float timeMultiplier;
    size_t cooksPerKitchen, replenishmentTime;

    try {
        get_program_arguments(argc, argv, timeMultiplier, cooksPerKitchen, replenishmentTime);

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return ERROR;
    }

    return EXIT_SUCCESS;
}
