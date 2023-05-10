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
#include "./include/UIManager.hpp"

size_t IDGenerator::currentID = 0;

int main(int argc, char* argv[])
{
    float timeMultiplier;
    size_t cooksPerKitchen, replenishmentTime;

    try {
        get_program_arguments(argc, argv, timeMultiplier, cooksPerKitchen, replenishmentTime);

        Reception reception(timeMultiplier, cooksPerKitchen, replenishmentTime);

        Process clientInputHandler([&reception]() { reception.interactive_shell_loop(); });

        // Process updatesDisplayHandler([&reception]() { reception.process_updates(); });

        // ! Wait for the child processes to finish
        // updatesDisplayHandler.wait();
        clientInputHandler.wait();

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return ERROR;
    }

    return EXIT_SUCCESS;
}

// #include <sys/wait.h>
// #include <unistd.h>
// #include <iostream>
// #include <string>
// #include <unordered_map>

// int main()
// {
//     const std::string pipeName = "/tmp/child_parent_pipe_0";
//     const std::string message = " 1 4 1 1 2 4";
//     const std::string message2 = " 1 4 1 1 2 4";

//     std::unordered_map<pid_t, std::unique_ptr<NamedPipeIPC>> _communicationPipes;

//     pid_t pid = fork();

//     if (pid < 0) {
//         std::cerr << "Fork failed!" << std::endl;
//         return 1;
//     }

//     if (pid > 0) {
//         // Parent process
//         _communicationPipes[pid] =
//             std::make_unique<NamedPipeIPC>(pipeName, NamedPipeIPC::Mode::Write);

//         _communicationPipes[pid]->write(message);
//         std::cout << "Parent: Sent message to child process" << std::endl;
//         _communicationPipes[pid]->write(message2);
//         std::cout << "Parent: Sent message to child process" << std::endl;

//         int status;
//         waitpid(pid, &status, 0);
//     } else {
//         // Child process
//         NamedPipeIPC ipc(pipeName, NamedPipeIPC::Mode::Read);
//         sleep(1);

//         std::string receivedMessage = ipc.read();
//         std::cout << "Child: Received message: " << receivedMessage << std::endl;

//         std::string receivedMessage1 = ipc.read();
//         std::cout << "Child: Received message: " << receivedMessage1 << std::endl;
//     }

//     return 0;
// }
