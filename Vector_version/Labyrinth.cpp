#include "Labyrinth.hpp"
#include <random>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iterator>

std::ostream& operator<<(std::ostream& os, const Labyrinth& lab)
{
    for (auto y = 0; y != lab.get().size(); ++y)
    {
        for (auto x = 0; x != lab.get()[y].size(); ++x)
        {
            if (lab.get()[y][x])
                os << BLOCK;
#ifdef _PRINT_WITH_EXIT_
            else if (!(lab.path()[y][x]))
                os << char(177);
#endif // _PRINT_WITH_EXIT
            else
                os << ' ';
        }
        os << std::endl;
    }
#ifndef _PRINT_WITHOUT_EXIT_
    std::cout << std::endl << "Exit the labyrinth\n";
    for (const auto& y : lab.path())
    {
        for (const auto& x : y)
        {
            if (x)
                os << BLOCK;
            else
                os << ' ';
        }
        os << std::endl;
    }
#endif // !_PRINT_WITHOUT_EXIT_
    return os;
}

Labyrinth::Labyrinth(size_t _width, size_t _height, Exist ex, size_t max_branch, size_t branch_rate, size_t branch_size) :
    branch_param { max_branch, branch_rate, branch_size }, width { _width / 2 * 2 }, height { _height / 2 * 2 },
    bBody(_height / 2 * 2, std::vector<bool>(_width / 2 * 2, true)), escape(_height / 2 * 2, std::vector<bool>(_width / 2 * 2, true))
{
    build_path(ex);
    build_subpath(ex);
}

std::multimap<size_t, size_t> Labyrinth::init_map()
{
    std::multimap<size_t, size_t> escape_map;
    for (size_t y = 2; y != height - 2; ++y)
        for (size_t x = 2; x != width - 2; ++x)
            if (!escape[y][x])
                escape_map.insert(std::pair<const size_t, size_t>{y, x});
    return escape_map;
}

void Labyrinth::build_subpath(Exist ex)
{
    std::multimap<size_t, size_t> escape_map = init_map();
    static std::default_random_engine e(static_cast<unsigned>(time(0)));
    std::bernoulli_distribution ber(branch_param.branch_rate / 1000.);
    std::vector<std::multimap<size_t, size_t>::value_type> sample_map {};
    std::sample(escape_map.cbegin(), escape_map.cend(), std::back_inserter(sample_map), (height * width) / (height + width), std::mt19937(std::random_device {}()));
#ifndef _FAST_BUILD_
    std::vector<std::pair<size_t, size_t>> ends {};
#endif // !_FAST_BUILD_
    //step 1
    for (auto& i : sample_map)
    {
        if (i.second % 2 == 0 && i.first % 2 == 0 && ber(e))
        {
#ifdef _FAST_BUILD_
            subpath(i.second, i.first, 0, sqrt(branch_param.max_size));
#endif // _FAST_BUILD_
#ifndef _FAST_BUILD_
            ends.emplace_back(subpath(i.second, i.first, ex));
#endif // !_FAST_BUILD_
        }
    }
    //step 2
#ifndef _FAST_BUILD_
    std::shuffle(ends.begin(), ends.end(), std::mt19937(std::random_device {}()));
    for (const auto& [x, y] : ends)
    {
        subpath(x, y, branch_param.max_branch);
    }
#endif // !_FAST_BUILD_
    //step 3 (final)
    if (ex == Exist::HORIZONTAL)
        for (size_t x = 3; x != width - 3; ++x)
        {
            for (size_t y = 3; y != height - 3; ++y)
            {
                if (!bBody[y][x] && ber(e) && x % 2 == 0 && y % 2 == 0)
                    subpath(x, y, branch_param.max_branch);
            }
        }
    else
        for (size_t y = 3; y != height - 3; ++y)
        {
            for (size_t x = 3; x != width - 3; ++x)
            {
                if (!bBody[y][x] && ber(e) && x % 2 == 0 && y % 2 == 0)
                    subpath(x, y, branch_param.max_branch);
            }
        }
}

std::pair<size_t, size_t> Labyrinth::subpath(size_t x, size_t y, Exist ex)
{
    static std::default_random_engine e(static_cast<unsigned>(time(0)));
    std::bernoulli_distribution d(0.5);
    std::uniform_int_distribution<> d_x;
    std::uniform_int_distribution<> d_y;
    uint8_t counter = 0;
    uint8_t status = 0;
    bool flag = true;
    int proc, delta;
    if (ex == Exist::HORIZONTAL)
    {
        d_x.param(std::uniform_int_distribution<>::param_type { -1, 1 });
        if (d(e))
            d_y.param(std::uniform_int_distribution<>::param_type { 0, 1 });
        else
            d_y.param(std::uniform_int_distribution<>::param_type { -1, 0 });
    }
    else
    {
        d_y.param(std::uniform_int_distribution<>::param_type { -1, 1 });
        if (d(e))
            d_x.param(std::uniform_int_distribution<>::param_type { 0, 1 });
        else
            d_x.param(std::uniform_int_distribution<>::param_type { -1, 0 });
    }
    while (counter < branch_param.max_size && flag)
    {
        proc = d_x(e);
        delta = d_y(e);
        if (proc && x + proc * STEP < width - 3 && x + proc * STEP > 3 && bBody[y][x + proc * STEP] && bBody[y - 1][x + proc * STEP] && bBody[y + 1][x + proc * STEP] && bBody[y][x + proc * (STEP + 1)] && bBody[y][x + proc])
        {
            x += proc * STEP;
            bBody[y][x - proc] = bBody[y][x] = false;
            status = 0;
            ++counter;
        }
        else if (proc)
        {
            ++status;
            if (status == STATUS_CUP) flag = false;
        }
        if (delta && y + delta * STEP > Y_LIMIT && y + delta * STEP < height - 2 && bBody[delta * STEP + y][x] && bBody[y + delta * STEP][x + 1] && bBody[y + delta * STEP][x - 1] && bBody[y + delta * (STEP + 1)][x] && bBody[y + delta][x])
        {
            y += delta * STEP;
            bBody[y - delta][x] = bBody[y][x] = false;
            status = 0;
            ++counter;
        }
        else if (delta)
        {
            ++status;
            if (status == STATUS_CUP) flag = false;
        }
    }
    return std::make_pair(x, y);
}

void Labyrinth::subpath(size_t x, size_t y, size_t re_count)
{
    static std::default_random_engine e(static_cast<unsigned>(time(0)));
    std::uniform_int_distribution<> d(-1, 1);
    std::bernoulli_distribution ber(branch_param.branch_rate / 1000.);
    uint8_t counter = 0;
    uint8_t status = 0;
    bool flag = true;
    int proc = d(e), delta = d(e);
    while (counter < branch_param.max_size && flag)
    {
        proc = d(e), delta = d(e);
        if (proc && x + proc * STEP < width - 3 && x + proc * STEP > 3 && bBody[y][x + proc * STEP] && bBody[y - 1][x + proc * STEP] && bBody[y + 1][x + proc * STEP] && bBody[y][x + proc * (STEP + 1)] && bBody[y][x + proc])
        {
            x += proc * STEP;
            bBody[y][x - proc] = bBody[y][x] = false;
            status = 0;
            ++counter;
        }
        else if (proc)
        {
            ++status;
            if (status == STATUS_CUP) flag = false;
        }
        if (delta && y + delta * STEP > Y_LIMIT && y + delta * STEP < height - 2 && bBody[delta * STEP + y][x] && bBody[y + delta * STEP][x + 1] && bBody[y + delta * STEP][x - 1] && bBody[y + delta * (STEP + 1)][x] && bBody[y + delta][x])
        {
            y += delta * STEP;
            bBody[y - delta][x] = bBody[y][x] = false;
            status = 0;
            ++counter;
        }
        else if (delta)
        {
            ++status;
            if (status == STATUS_CUP) flag = false;
        }
        if (ber(e) && re_count && !status && y % 2 == 0 && x % 2 == 0)
        {
            subpath(x, y, re_count - 1);
        }
    }
}

bool Labyrinth::hor_check(size_t indx, size_t indy, short course) const
{
    for (; indx < width / 2 + width / 2 * course; indx += course)
        if (!bBody[indy][indx])
            return true;
    return false;
}

bool Labyrinth::ver_check(size_t indx, size_t indy, short course) const
{
    for (; indy < height / 2 + height / 2 * course; indy += course)
        if (!bBody[indy][indx])
            return true;
    return false;
}

void Labyrinth::build_frames()
{
    for (auto& y : bBody)
        for (auto x : y)
            x = true;
    for (auto& y : escape)
        for (auto x : y)
            x = true;
}

void Labyrinth::build_path(Exist ex)
{
    static std::default_random_engine e(static_cast<unsigned>(time(0)));
    std::uniform_int_distribution<> delta(-1, 1);
    std::bernoulli_distribution proc(0.9);
    size_t counter = 0;
    bool flag = true;
    size_t x, y;
    bool status;
    int dy;//dy - delta y
    int dx;//dx - delta x
    if (ex == Exist::HORIZONTAL)
    {
        std::bernoulli_distribution cf(std::clamp(1.56 - pow(static_cast<double>(height) / width, COEFFICIENT), 0.1, 1.));//cf - curvature factor
        while (flag)
        {
            x = 0, y = ((height / 2) % 2) ? height / 2 + 1 : height / 2;
            escape[y][x + 1] = bBody[y][x + 1] = escape[y][x] = bBody[y][x] =
                escape[y][x + 2] = bBody[y][x + 2] = false;
            x += 2;
            while (x < width - 2 && counter != width * height)
            {
                ++counter;
                status = proc(e);
                dx = delta(e);
                if (dx == -1 && cf(e) || counter < 6)
                    dx = 1;
                dy = delta(e);
                if (status && dx && x + dx * 2 > 2 && x + dx * 2 < width &&
                    escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2])
                {
                    x += dx * 2;
                    escape[y][x - dx] = bBody[y][x - dx] =
                        escape[y][x] = bBody[y][x] = false;
                    if (proc(e) && x + dx * 2 > 2 && x + dx * 2 < width &&
                        escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2])
                    {

                        x += dx * 2;
                        escape[y][x - dx] = bBody[y][x - dx] =
                            escape[y][x] = bBody[y][x] = false;
                    }
                }
                else if (dx)
                {
                    dx = -dx;
                    if (x + dx * 2 > 2 && x + dx * 2 < width &&
                        escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2] &&
                        escape[y - 1][x + dx * 2])
                    {
                        x += dx * 2;
                        escape[y][x - dx] = bBody[y][x - dx] =
                            escape[y][x] = bBody[y][x] = false;
                        if (proc(e) && x + dx * 2 > 2 && x + dx * 2 < width &&
                            escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2] &&
                            escape[y - 1][x + dx * 2])
                        {

                            x += dx * 2;
                            escape[y][x - dx] = bBody[y][x - dx] =
                                escape[y][x] = bBody[y][x] = false;
                        }
                    }
                }

                if (dy && y + dy * STEP > 2 && y + dy * STEP < height - 2 && escape[y + dy * STEP][x] &&
                    escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
                {
                    y += dy * STEP;
                    escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
                    if (!cf(e) && y + dy * STEP > 2 && y + dy * STEP < height - 2 && escape[y + dy * STEP][x] &&
                        escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
                    {
                        y += dy * STEP;
                        escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
                    }
                }
                else if (dy)
                {
                    dy = -dy;
                    if (y + dy * STEP > 2 && y + dy * STEP < height - 2 && escape[y + dy * STEP][x] &&
                        escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
                    {
                        y += dy * STEP;
                        escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
                        if (!cf(e) && y + dy * STEP > 2 && y + dy * STEP < height - 2 && escape[y + dy * STEP][x] &&
                            escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
                        {
                            y += dy * STEP;
                            escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
                        }
                    }
                }
            }
            if (counter != width * height)
            {
                flag = false;
            }
            else
            {
                build_frames();
                counter = 0;
            }
        }
        escape[y][x + 1] = bBody[y][x + 1] = false;
    }
    else
    {
        std::bernoulli_distribution cf(std::clamp(1.56 - pow(static_cast<double>(width) / height, COEFFICIENT), 0.1, 1.));
        while (flag)
        {
            x = ((width / 2) % 2) ? width / 2 + 1 : width / 2, y = 0;
            escape[y + 1][x] = bBody[y + 1][x] = escape[y][x] = bBody[y][x] =
                escape[y + 2][x] = bBody[y + 2][x] = false;
            y += 2;
            while (y < height - 2 && counter != width * height)
            {
                ++counter;
                status = proc(e);
                dy = delta(e);
                if (dy == -1 && cf(e) || counter < 3)
                    dy = 1;
                dx = delta(e);
                if (status && dy && y + dy * 2 > 1 && y + dy * 2 < height &&
                    escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
                {
                    y += dy * 2;
                    escape[y - dy][x] = bBody[y - dy][x] =
                        escape[y][x] = bBody[y][x] = false;
                    if (proc(e) && y + dy * 2 > 1 && y + dy * 2 < height &&
                        escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
                    {
                        y += dy * 2;
                        escape[y - dy][x] = bBody[y - dy][x] =
                            escape[y][x] = bBody[y][x] = false;
                    }
                }
                else if (dy)
                {
                    dy = -dy;
                    if (y + dy * 2 > 1 && y + dy * 2 < height &&
                        escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
                    {
                        y += dy * 2;
                        escape[y - dy][x] = bBody[y - dy][x] =
                            escape[y][x] = bBody[y][x] = false;
                        if (proc(e) && y + dy * 2 > 1 && y + dy * 2 < height &&
                            escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
                        {
                            y += dy * 2;
                            escape[y - dy][x] = bBody[y - dy][x] =
                                escape[y][x] = bBody[y][x] = false;
                        }
                    }
                }
                if (dx && x + dx * STEP > 2 && x + dx * STEP < width - 3 && escape[y][x + dx * STEP] &&
                    escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
                {
                    x += dx * STEP;
                    escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
                    if (!cf(e) && x + dx * STEP > 2 && x + dx * STEP < width - 3 && escape[y][x + dx * STEP] &&
                        escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
                    {
                        x += dx * STEP;
                        escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
                    }
                }
                else if (dx)
                {
                    dx = -dx;
                    if (x + dx * STEP > 2 && x + dx * STEP < width - 3 && escape[y][x + dx * STEP] &&
                        escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
                    {
                        x += dx * STEP;
                        escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
                        if (!cf(e) && x + dx * STEP > 2 && x + dx * STEP < width - 3 && escape[y][x + dx * STEP] &&
                            escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
                        {
                            x += dx * STEP;
                            escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
                        }
                    }
                }
            }
            if (counter != width * height)
            {
                flag = false;
            }
            else
            {
                build_frames();
                counter = 0;
            }
        }
        escape[y + 1][x] = bBody[y + 1][x] = false;
    }
    init_map();
}

void Labyrinth::regenerate(Exist ex)
{
    build_frames();
    build_path(ex);
    build_subpath(ex);
}

void Labyrinth::regenerate(Exist ex, size_t _width, size_t _height, size_t max_branch, size_t branch_rate, size_t branch_size)
{
    width = _width;
    height = _height;
    branch_param.max_branch = max_branch;
    branch_param.branch_rate = branch_rate;
    branch_param.max_size = branch_size;
    build_frames();
    build_path(ex);
    build_subpath(ex);
}