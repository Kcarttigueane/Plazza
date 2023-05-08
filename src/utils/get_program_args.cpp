/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** get_program_args.cpp
*/

#include "MyCustomException.hpp"
#include "Plazza.hpp"

void get_program_arguments(int argc, char* argv[], float& timeMultiplier, size_t& cooksPerKitchen,
                           size_t& replenishmentTime)
{
    if (argc != 4)
        throw MyCustomException(USAGE, __FILE__, __func__);

    timeMultiplier = std::stof(argv[1]);
    cooksPerKitchen = std::stoi(argv[2]);
    replenishmentTime = std::stoi(argv[3]);

    if (timeMultiplier <= 0.0f) {
        throw MyCustomException("time_multiplier must be superior to 0", __FILE__, __func__);
    }

    if (cooksPerKitchen <= 0) {
        throw MyCustomException("cooks_per_kitchen must be a positive integer", __FILE__, __func__);
    }

    if (replenishmentTime <= 0) {
        throw MyCustomException("replenishment_time must be a positive integer", __FILE__,
                                __func__);
    }
}
