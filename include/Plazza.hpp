/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Plazza.hpp
*/

#pragma once

#include <cstdio>
#include <iostream>

#include "Colors.hpp"

#define USAGE "USAGE: ./plazza [multiplier] [cooks_per_kitchen] [time]\n"

#define PROMPT "➜ "

enum ErrorCodes {
    FAILURE = -1,
    ERROR = 84,
};

enum MessageType {
    PizzaOrderRequest,
    StatusRequest,
    StatusResponse,
    PizzaOrderResponse,
};

// ! My includes:

void get_program_arguments(int argc, char* argv[], float& timeMultiplier,
                           std::size_t& cooksPerKitchen, std::size_t& replenishmentTime);

void appendToFile(const std::string& filename, const std::string& data);

std::string getCurrentTimeString();
