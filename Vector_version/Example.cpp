#define _PRINT_WITHOUT_EXIT_
#define _PRINT_WITH_EXIT_
#include "Labyrinth.hpp"
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
    std::cout << "Example code of vector version without template" << std::endl;
    size_t qtsubranch, branchiness, lnbranch, width, height;
    int exist;
    while (true)
    {
        std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n Exist(0 - vertical, 1 - horizontal\n or -1, if you want use default parameters): ";
        std::cin >> exist;
        assert(exist <= 1);
        if (exist == -1)
        {
            width = WIDTH;
            height = HEIGHT;
            exist = 1;
            qtsubranch = 100;
            branchiness = 1000;
            lnbranch = 100;
        }
        else
        {
            std::cout << " Width: ";
            std::cin >> width;
            std::cout << " Height: ";
            std::cin >> height;
            std::cout << " Maximum number of branches in sub-branches: ";
            std::cin >> qtsubranch;
            std::cout << " Branchiness(1 - 1000): ";
            std::cin >> branchiness;
            assert(branchiness >= 1 && branchiness <= 1000);
            std::cout << " Branch length: ";
            std::cin >> lnbranch;
        }
        std::cout << " Labyrinth in progress...";
        Labyrinth lab(width, height, static_cast<Exist>(exist), qtsubranch, branchiness, lnbranch);
        std::cout << "\n Labyrinth generated:\n";
        std::cout << lab << std::endl;
    }
}