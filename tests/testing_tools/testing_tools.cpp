#include "testing_tools.hpp"
#include <fstream>
#include <iostream>

test::pcout::~pcout()
{
    if (file.empty())
    {
        std::lock_guard<std::mutex> l { cout_mutex };
        std::cout << rdbuf();
        std::cout.flush();
        return;
    }
    std::lock_guard<std::mutex> l { ofs_mutex };
    std::ofstream ofs { file, std::ios::app };
    ofs << rdbuf();
    ofs.flush();
    ofs.close();
}

test::pcout::pcout(const std::string& _File) :
    std::stringstream {}, file { _File }{}

