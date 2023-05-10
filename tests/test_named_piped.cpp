/*
** EPITECH PROJECT, 2022
** B-CCP-400-LYN-4-1-theplazza-jules.dutel
** File description:
** test_named_piped.cpp
*/

#include <criterion/criterion.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/NamePipeIPC.hpp"

Test(NamedPipeIPC, InterprocessCommunication)
{
    const std::string pipeName = "/tmp/named_pipe_test";
    const std::string message = "Test message";

    pid_t pid = fork();

    if (pid < 0) {
        cr_assert(false);
    }

    if (pid == 0) {  // Child process
        NamedPipeIPC ipc(pipeName, NamedPipeIPC::Mode::Read);
        std::string receivedMessage = ipc.read();
        cr_assert(receivedMessage == message);

        _Exit(0);
    } else {
        NamedPipeIPC ipc(pipeName, NamedPipeIPC::Mode::Write);
        ipc.write(message);

        int status;
        waitpid(pid, &status, 0);
    }
}
