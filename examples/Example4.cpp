#include "../Vector_version/Labyrinth.hpp"
#include <cassert>
#include <memory>
#include <iomanip>
#include <iostream>
#define WIDTH 180
#define HEIGHT 80

int main(int argv, char** argc)
{
    std::cout << "Labirint generator" << std::setw(13) << " 2.2.0\n" << std::endl;
    std::cout << "2020" << std::setw(40) << " Damir Hismatov(c)\n\n";
    std::cout << "Example code #4 of vector version without template" << std::endl;
    size_t _Seed, width, height;
    int ex;
    if (argv == 4)
    {
        ex = std::stoull(argc[1]);
        width = std::stoull(argc[2]);
        height = std::stoull(argc[3]);
        std::cout << " Labyrinth in progress...";
        Labyrinth lab(static_cast<Labyrinth::exist>(ex), width, height);
        std::cout << "\n Labyrinth generated:\n";
        //std::cout << lab << std::endl;
        std::cout << "Seed: " << lab.seed() << std::endl;
        return 0;
    }
    //while (true)
    {
        std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n exist(0 - vertical, 1 - horizontal\n or -1, if you want use default parameters): ";
        std::cin >> ex;
        assert(ex <= 1);
        if (ex == -1)
        {
            width = WIDTH;
            height = HEIGHT;
            ex = 1;
        }
        else
        {
            std::cout << " Width: ";
            std::cin >> width;
            std::cout << " Height: ";
            std::cin >> height;
        }
        std::cout << " Labyrinth in progress...";
        Labyrinth lab(static_cast<Labyrinth::exist>(ex), width, height);
        std::cout << "\n Labyrinth generated:\n";
        std::cout << lab << std::endl;
        std::cout << "Seed: " << lab.seed() << std::endl;
    }
}