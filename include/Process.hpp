/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Process.hpp
*/

#pragma once

#include <sys/wait.h>
#include <functional>

#include "Plazza.hpp"

class Process {
  private:
    pid_t pid;

  public:
    explicit Process(const std::function<void()>& func)
    {
        pid = fork();
        if (pid == 0) {
            func();
            exit(0);
        } else if (pid == FAILURE) {
            std::cerr << "Error creating process." << std::endl;
            exit(1);
        }
    }

    void wait() const
    {
        int status;
        waitpid(pid, &status, 0);
    }

    [[nodiscard]] pid_t getPid() const { return pid; }
};
