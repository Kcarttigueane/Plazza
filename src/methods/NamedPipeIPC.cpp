/*
** EPITECH PROJECT, 2022
** B-CCP-400-LYN-4-1-theplazza-jules.dutel
** File description:
** NamedPipeIPC.cpp
*/

#include "NamedPipeIPC.hpp"

#include <memory>
#include <sstream>

void NamedPipeIPC::write(const std::string& message)
{
    if (_mode != Mode::Write) {
        throw std::runtime_error("NamedPipeIPC is not in write mode");
    }

    ssize_t bytesWritten = ::write(_pipeFd, message.c_str(), message.size());

    // std::cout << BLUE_TEXT("[WRITE]: ") << RED_TEXT(_pipeName) << " : \"" GREEN_TEXT(message)
    //           << "\"" << std::endl;  // ** DEBUG

    if (bytesWritten != static_cast<ssize_t>(message.size())) {
        throw std::runtime_error("Failed to write the complete message to the named pipe");
    }

    ::write(_pipeFd, &DELIMITER, 1);
}

std::string NamedPipeIPC::read()
{
    if (_mode != Mode::Read) {
        throw std::runtime_error("NamedPipeIPC is not in read mode");
    }

    std::unique_ptr<char[]> buffer(new char[_bufferSize]());
    std::ostringstream message;

    char ch;
    while (::read(_pipeFd, &ch, 1) > 0 && ch != DELIMITER) {
        message << ch;
    }

    // std::cout << BLUE_TEXT("[READ]: ") << RED_TEXT(_pipeName) << " : \"" GREEN_TEXT(message.str())
    //           << "\"" << std::endl;  // ** DEBUG

    if (ch != DELIMITER) {
        throw std::runtime_error("Failed to read the complete message from the named pipe");
    }

    return message.str();
}
