#include "../Vector_version/Labyrinth.hpp"
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
    std::cout << "Example code #2 of vector version without template" << std::endl;
    size_t _Seed, width, height;
    int ex;
    //while (true)
    {
        std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n exist(0 - vertical, 1 - horizontal\n or -1, if you want use default parameters): ";
        std::cin >> ex;
        assert(ex <= 1);
        if (ex == -1)
        {
            width = WIDTH;
            height = HEIGHT;
            _Seed = std::random_device {}();
            std::cout << " Seed: " << _Seed << std::endl;
            ex = 1;
        }
        else
        {
            std::cout << " Seed: ";
            std::cin >> _Seed;
            std::cout << " Width: ";
            std::cin >> width;
            std::cout << " Height: ";
            std::cin >> height;
        }
        std::cout << " Labyrinth in progress...";
        Labyrinth lab(static_cast<Labyrinth::exist>(ex), _Seed, width, height);
        std::cout << "\n Labyrinth generated:\n";
        std::cout << lab << std::endl;

    }
}