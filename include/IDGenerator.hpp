/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** IDGenerator.hpp
*/

#include <cstdint>
#include <iostream>

class IDGenerator {
  public:
    static size_t generateID() { return ++currentID; }

  private:
    static size_t currentID;
};
