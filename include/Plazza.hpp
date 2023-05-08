/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Plazza.hpp
*/

#pragma once

#include <termios.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
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
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>

#define USAGE "USAGE: ./plazza [multiplier] [cooks_per_kitchen] [time]\n"

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
