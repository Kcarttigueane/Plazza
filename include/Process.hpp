/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Process.hpp
*/

#pragma once

#include <sys/wait.h>
#include <unistd.h>
#include <condition_variable>
#include <functional>
#include <iostream>

class Process {
  private:
    pid_t pid;

  public:
    Process(std::function<void()> func)
    {
        pid = fork();
        if (pid == 0) {
            func();
            exit(0);
        } else if (pid == -1) {
            std::cerr << "Error creating process." << std::endl;
            exit(1);
        }
    }

    void wait()
    {
        int status;
        waitpid(pid, &status, 0);
    }

    pid_t getPid() const { return pid; }
};
