#include "Labyrinth.h"

int main()
{
	Labyrinth<90, 40> lab{ Exist::HORIZONTAL, 10, 500, 30 };
	std::cout << lab << std::endl;
	lab.regenerate();
	std::cout << lab;
}