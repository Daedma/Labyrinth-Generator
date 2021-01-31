#include "Vector_version\\Labyrinth.hpp"
#include <cassert>
#include <memory>
#include <iomanip>
#include <iostream>
#define WIDTH 180
#define HEIGHT 80

int main()
{
    std::cout << "Labirint generator" << std::setw(13) << " 2.0.0\n" << std::endl;
    std::cout << "2020" << std::setw(40) << " Damir Hismatov(c)\n\n";
    std::cout << "Example code #3 of vector version without template" << std::endl;
    std::string params;
    //while (true)
    {
        std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n seed: ";
        std::cin >> params;
        std::cout << " Labyrinth in progress...";
        Labyrinth lab(params);
        std::cout << "\n Labyrinth generated:\n";
        std::cout << lab << std::endl;

    }
}