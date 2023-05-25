/*
** EPITECH PROJECT, 2022
** B-CCP-400-LYN-4FAILURE-theplazza-jules.dutel
** File description:
** NamedPipeIPC.hpp
*/

#pragma once

#include <utility>

#include "Plazza.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

constexpr size_t BUFFER_SIZE = 256;
constexpr char DELIMITER = '\0';

/**
** @class NamedPipeIPC
** @brief A C++ class for interprocess communication using Named Pipes.
**
** This class provides a simple interface for IPC using Named Pipes in C++. It supports
** reading and writing messages between processes and automatically manages the underlying
** named pipe file.
**/
class NamedPipeIPC {
  public:
    /**
    ** @enum Mode
    ** @brief The mode in which the NamedPipeIPC object operates.
    **/
    enum class Mode { Read, Write };

    /**
    ** @brief Constructor for the NamedPipeIPC class.
    **
    ** Initializes a NamedPipeIPC object with the specified named pipe and mode.
    ** The named pipe file is created if it does not exist.
    **
    ** @param pipeName The name of the named pipe file.
    ** @param mode The mode in which the NamedPipeIPC object operates (Read or Write).
    ** @param bufferSize The size of the buffer used for reading messages (default is BUFFER_SIZE).
    **/
    NamedPipeIPC(std::string pipeName, Mode mode, size_t bufferSize = BUFFER_SIZE)
        : _pipeName(std::move(pipeName)), _mode(mode), _pipeFd(FAILURE), _bufferSize(bufferSize)
    {
        if (access(_pipeName.c_str(), F_OK) == FAILURE) {
            mkfifo(_pipeName.c_str(), 0666);
        }

        try {
            openPipe();
        } catch (std::runtime_error& e) {
            std::remove(_pipeName.c_str());
            throw;
        }
    }

    /**
    ** @brief Destructor for the NamedPipeIPC class.
    **
    ** Closes the named pipe file and removes it from the filesystem.
    **/
    ~NamedPipeIPC()
    {
        closePipe();
        if (_mode == Mode::Write) {
            std::remove(_pipeName.c_str());
        }
    }

    // ! Getter:

    [[nodiscard]] std::string getPipeName() const { return _pipeName; }

    [[nodiscard]] int getPipeFd() const { return _pipeFd; }

    [[nodiscard]] Mode getMode() const { return _mode; }

    // ! Methods

    /**
    ** @brief Writes a message to the named pipe.
    **
    ** Writes the specified message to the named pipe. The NamedPipeIPC object must be
    ** in Write mode, otherwise an exception is thrown.
    **
    ** @param message The message to write to the named pipe.
    ** @throw std::runtime_error if the NamedPipeIPC object is not in Write mode or if writing fails.
    **/
    void write(const std::string& message);

    /**
    ** @brief Reads a message from the named pipe.
    **
    ** Reads a message from the named pipe into a std::string. The NamedPipeIPC object must be
    ** in Read mode, otherwise an exception is thrown.
    **
    ** @return The message read from the named pipe.
    ** @throw std::runtime_error if the NamedPipeIPC object is not in Read mode or if reading fails.
    **/
    std::string read();

  private:
    void openPipe()
    {
        int flags = (_mode == Mode::Read) ? O_RDONLY : O_WRONLY;
        _pipeFd = open(_pipeName.c_str(), flags);

        // std::cout << BLUE_TEXT("[OPEN]: ") << RED_TEXT(_pipeName) << std::endl;  // ** DEBUG

        if (_pipeFd < 0) {
            throw std::runtime_error("Failed to open the named pipe");
        }
    }

    void closePipe()
    {
        if (_pipeFd != FAILURE) {
            close(_pipeFd);
            _pipeFd = FAILURE;
        }
    }

    std::string _pipeName;
    Mode _mode;
    int _pipeFd;
    std::size_t _bufferSize;
};
