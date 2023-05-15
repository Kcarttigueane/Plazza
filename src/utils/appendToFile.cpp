/*
** EPITECH PROJECT, 2022
** Plazza
** File description:
** appendToFile.cpp
*/

#include <fstream>
#include <iostream>
#include <string>

void appendToFile(const std::string& filename, const std::string& data)
{
    std::ofstream outFile(filename, std::ios::app);

    if (!outFile) {
        std::cerr << "Error: Could not open the file for appending." << std::endl;
        return;
    }

    outFile << data << std::endl;
    outFile.close();
}
