#include "testing_tools/testing_tools.hpp"
#include "lab_test.hpp"
#include "../Labyrinth.hpp"
#include <vector>
#include <thread>
#include <map>
#include <fstream>
#include <iostream>

#ifndef ACCURACY
#define ACCURACY 6ULL
#endif // !ACCURACY

void labyrinth_test(size_t _Step, size_t _Threads, range _Range, std::string _File)
{
    std::mutex map_mut;
    size_t cur_step { _Range.from };
    std::map<size_t, double> results;
    std::vector<size_t> step_values;
    for (auto i = 0; i != _Threads; ++i)
    {
        step_values.push_back(_Range.from + (_Step * i));
    }
    _Step *= _Threads;

    auto lab_construct = [](size_t szLab){
        return test::time_for_construct<Labyrinth>(Labyrinth::exits::hor, szLab, szLab);
    };

    auto bench = [_File, lab_construct, &results, &map_mut](size_t szLab){
        auto val = test::averaged_result(ACCURACY, lab_construct, szLab);
        test::pcout {} << "(" << szLab << "; " << val << ")\n";
        std::lock_guard<std::mutex> l { map_mut };
        results[szLab] = val;
    };

    std::vector<std::thread> threads;
    while (cur_step <= _Range.to)
    {
        for (auto cells : step_values)
        {
            threads.emplace_back(bench, cells);
        }
        for (auto& th : threads)
        {
            th.join();
        }
        threads.clear();
        for (auto it = step_values.rbegin(); it != step_values.rend(); ++it)
        {
            *it += _Step;
            if (*it > _Range.to)
            {
                step_values.pop_back();
                it = step_values.rbegin();
            }
        }
        cur_step += _Step;
    }
    std::ofstream ofs { _File };
    while (!ofs)
    {
        std::cout << "The file was not successful. Please enter the correct file name for recording: ";
        std::cin >> _File;
        ofs.close();
        ofs.clear();
        ofs.open(_File);
    }
    std::cout << "Starts entry to the file" << std::endl;
    for (auto& i : results)
    {
        ofs << "(" << i.first << "; " << i.second << ")\n";
    }
    ofs.close();
    std::cout << "Record ended" << std::endl;
}