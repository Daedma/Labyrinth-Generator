#include "lab_test.hpp"

int main(int argv, char** argc)
{
    if (argv == 6)
    {
        size_t step = std::stoull(argc[1]);
        size_t threads = std::stoull(argc[2]);
        size_t from = std::stoull(argc[3]);
        size_t to = std::stoull(argc[4]);
        labyrinth_test(step, threads, { from, to }, argc[5]);
        return 0;
    }
    return -1;
}