/*
** EPITECH PROJECT, 2022
** B-CCP-400-LYN-4FAILURE-theplazza-jules.dutel
** File description:
** NamePipeIPC.hpp
*/

#pragma once

#include "Plazza.hpp"

constexpr size_t BUFFER_SIZE = 1024;
constexpr char DELIMITER = '\0';

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

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
    NamedPipeIPC(const std::string& pipeName, Mode mode, size_t bufferSize = BUFFER_SIZE)
        : _pipeName(pipeName), _mode(mode), _pipeFd(FAILURE), _bufferSize(bufferSize)
    {
        if (access(_pipeName.c_str(), F_OK) == FAILURE) {
            mkfifo(_pipeName.c_str(), 0666);
        }
        openPipe();
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

    NamedPipeIPC& operator=(const NamedPipeIPC&) = delete;

    // ! Getter:

    std::string getPipeName() const { return _pipeName; }

    int getPipeFd() const { return _pipeFd; }

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
    void write(const std::string& message)
    {
        if (_mode != Mode::Write) {
            throw std::runtime_error("NamedPipeIPC is not in write mode");
        }

        ssize_t bytesWritten = ::write(_pipeFd, message.c_str(), message.size());

        std::cout << GREEN_TEXT("write: ") << "\"" GREEN_TEXT(message)
                  << "\" : " << RED_TEXT(_pipeName) << std::endl;

        if (bytesWritten != static_cast<ssize_t>(message.size())) {
            throw std::runtime_error("Failed to write the complete message to the named pipe");
        }

        ::write(_pipeFd, &DELIMITER, 1);
    }

    /**
    ** @brief Reads a message from the named pipe.
    **
    ** Reads a message from the named pipe into a std::string. The NamedPipeIPC object must be
    ** in Read mode, otherwise an exception is thrown.
    **
    ** @return The message read from the named pipe.
    ** @throw std::runtime_error if the NamedPipeIPC object is not in Read mode or if reading fails.
    **/
    std::string read()
    {
        if (_mode != Mode::Read) {
            throw std::runtime_error("NamedPipeIPC is not in read mode");
        }

        std::unique_ptr<char[]> buffer(new char[_bufferSize]);

        ssize_t bytesRead = ::read(_pipeFd, buffer.get(), _bufferSize);

        if (bytesRead < 0) {
            throw std::runtime_error("Failed to read from the named pipe");
        }

        return std::string(buffer.get(), bytesRead);
    }

  private:
    void openPipe()
    {
        int flags = (_mode == Mode::Read) ? O_RDONLY : O_WRONLY;
        _pipeFd = open(_pipeName.c_str(), flags);

        std::cout << BLUE_TEXT("Open: ") << RED_TEXT(_pipeName) << std::endl;

        if (_pipeFd < 0) {
            throw std::runtime_error("Failed to open the named pipe");
        }
    }

    void closePipe()
    {
        if (_pipeFd != -1) {
            close(_pipeFd);
            _pipeFd = -1;
        }
    }

    std::string _pipeName;
    Mode _mode;
    int _pipeFd;
    std::size_t _bufferSize;
};
