#include "Labyrinth.h"
#include <cassert>
#include <memory>
#include <iomanip>
#define WIDTH 150
#define HEIGHT 40

int main()
{
	std::cout << "Labirint generator" << std::setw(13) << " 1.0.0\n" << std::endl;
	std::cout << "2020" << std::setw(40) << " Damir Hismatov(c)\n\n";
	std::cout << "Constant size of labyrinth is " << WIDTH << 'x' << HEIGHT << std::endl;
	size_t exist, qtsubranch, branchiness, lnbranch;
	while (true)
	{
		std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n Exist(0 - vertical, 1 - horizontal): ";
		std::cin >> exist;
		assert(exist <= 1);
		std::cout << "Maximum number of branches in sub-branches: ";
		std::cin >> qtsubranch;
		std::cout << "Branchiness(1 - 1000): ";
		std::cin >> branchiness;
		assert(branchiness >= 1 && branchiness <= 1000);
		std::cout << "Branch length: ";
		std::cin >> lnbranch;
		std::cout << "Labyrinth in progress...\n";
		std::unique_ptr<Labyrinth<WIDTH, HEIGHT>> lab(std::make_unique<Labyrinth<WIDTH, HEIGHT>>(static_cast<Exist>(exist), qtsubranch, branchiness, lnbranch));
		std::cout << "Labyrinth generated:\n";
		std::cout << *lab << std::endl;
	}
}