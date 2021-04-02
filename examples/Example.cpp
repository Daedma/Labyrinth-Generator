#define _PRINT_WITHOUT_EXIT_
#define _PRINT_WITH_EXIT_
#include "../Labyrinth.h"
#include <cassert>
#include <memory>
#include <iomanip>
#define WIDTH 236
#define HEIGHT 80

int main()
{
	std::cout << "Labirint generator" << std::setw(13) << " 1.1.2\n" << std::endl;
	std::cout << "2020" << std::setw(40) << " Damir Hismatov(c)\n\n";
	std::cout << "Constant size of labyrinth is " << WIDTH << 'x' << HEIGHT << std::endl;
	size_t qtsubranch, branchiness, lnbranch;
	int ex;
	while (true)
	{
		std::cout << "\n\nPlease, enter parameters to generate the labyrinth or press [CTRL]+[C] to exit...\n exist(0 - vertical, 1 - horizontal\n or -1, if you want use default parameters): ";
		std::cin >> ex;
		assert(ex <= 1);
		if (ex == -1)
		{
			ex = 1;
			qtsubranch = 100;
			branchiness = 1000;
			lnbranch = 100;
		}
		else
		{
			std::cout << " Maximum number of branches in sub-branches: ";
			std::cin >> qtsubranch;
			std::cout << " Branchiness(1 - 1000): ";
			std::cin >> branchiness;
			assert(branchiness >= 1 && branchiness <= 1000);
			std::cout << " Branch length: ";
			std::cin >> lnbranch;
		}
		std::cout << " Labyrinth in progress...";
		std::unique_ptr<Labyrinth<WIDTH, HEIGHT>> lab(std::make_unique<Labyrinth<WIDTH, HEIGHT>>(static_cast<exist>(ex), qtsubranch, branchiness, lnbranch));
		std::cout << "\n Labyrinth generated:\n";
		std::cout << *lab << std::endl;
	}
}