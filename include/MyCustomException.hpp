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

using std::string;

class MyCustomException : public std::exception {
  private:
    string _message;
    string _file;
    string _function;
    string _fullMessage;

  public:
    MyCustomException(const string& message, const string& file, const string& function)
        : _message(message), _file(file), _function(function)
    {
        _fullMessage = _message + " (file: " + _file + ", function: " + _function + ")";
    }

    const char* what() const noexcept override { return _fullMessage.c_str(); }
};

#endif  // MY_CUSTOM_EXCEPTION_H
