/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Plazza.hpp
*/

#pragma once

#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Colors.hpp"

#define USAGE "USAGE: ./plazza [multiplier] [cooks_per_kitchen] [time]\n"

#define PROMPT "➜ "

enum ErrorCodes {
    SUCCESS = 0,
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

void get_program_arguments(int argc, char* argv[], float& timeMultiplier, size_t& cooksPerKitchen,
                           size_t& replenishmentTime);

void appendToFile(const std::string& filename, const std::string& data);

std::string getCurrentTimeString();
