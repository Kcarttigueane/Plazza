/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** Process.hpp
*/

#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

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

class Thread {
  private:
    std::thread t;

  public:
    Thread() {}

    int create(pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void*),
               void* arg)
    {
        return pthread_create(thread, attr, start_routine, arg);
    }

    int join(pthread_t thread, void** retval) { return pthread_join(thread, retval); }
};

class Mutex {
  public:
    Mutex() { pthread_mutex_init(&m_mutex, nullptr); }

    ~Mutex() { pthread_mutex_destroy(&m_mutex); }

    int lock() { return pthread_mutex_lock(&m_mutex); }

    int unlock() { return pthread_mutex_unlock(&m_mutex); }

  private:
    pthread_mutex_t m_mutex;
};

class CondVar {
  public:
    CondVar() { pthread_cond_init(&m_cond_var, nullptr); }

    ~CondVar() { pthread_cond_destroy(&m_cond_var); }

    int wait(pthread_mutex_t* mutex) { return pthread_cond_wait(&m_cond_var, mutex); }

    int signal() { return pthread_cond_signal(&m_cond_var); }

    int broadcast() { return pthread_cond_broadcast(&m_cond_var); }

  private:
    pthread_cond_t m_cond_var;
};
