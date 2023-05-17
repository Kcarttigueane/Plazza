/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** MyCustomException.hpp
*/

#ifndef MY_CUSTOM_EXCEPTION_H
#define MY_CUSTOM_EXCEPTION_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

using std::string;

class MyCustomException : public std::exception {
  private:
    string _message;
    string _file;
    string _function;
    string _fullMessage;

  public:
    MyCustomException(string  message, string  file, string  function)
        : _message(std::move(message)), _file(std::move(file)), _function(std::move(function))
    {
        _fullMessage = _message + " (file: " + _file + ", function: " + _function + ")";
    }

    [[nodiscard]] const char* what() const noexcept override { return _fullMessage.c_str(); }
};

#endif  // MY_CUSTOM_EXCEPTION_H
