/*
* Header containing Labyrinth generator
* version 1.1.2
* Author: Damir Hismatov
* Github: https://github.com/Daedma
*/
#pragma once
#include <array>
#include <iostream>
#include <random>
#include <map>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <vector>

#define BLOCK char(219)//aggregate for print walls of labyrinth
#ifndef STATUS_CUP
	#define STATUS_CUP 5 
/*Maximum amount of fails when generating subbranches
 *Recommended values STATUS_CUP >= 4
 *Affects the speed of construction
 */
#endif // !STATUS_CUP

#ifndef COEFFICIENT
	#define COEFFICIENT 0.25
/*As you increase, the curvature of the exit path decreases.
 *When decreasing, it increases.
 *Increase if there are problems with build time.
 */
#endif // !COEFFICIENT

//Service
//Don't change this
#define Y_LIMIT 1
#define STEP 2//step

/*
* VERTICAL - means building a labyrinth with exits on the sides
* HORIZONTAL - means building a labyrinth with exits from above and below
*/
enum class Exist//parameters to pass to constructor and function regeneration
{
	VERTICAL, //exits on the sides
	HORIZONTAL //exits from above and below
};

//Class for random generation of labyrinth
template <unsigned W, unsigned H>//width and height
class Labyrinth
{
	/*
	* y default displays Labyrinth and path separately
	* use macros _PRINT_WITHOUT_EXIT_ , if you want print labyrinth without exit path separately
	* use macros _PRINT_WITH_EXIT_ , if you want highlight exit inside labyrinth
	*/
	friend std::ostream& operator<<(std::ostream& os, const Labyrinth<W, H>& lab)
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
		for (const auto& y : lab.escape)
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
	template<unsigned Width, unsigned Height, typename T>
	using _2dArray = std::array<std::array<T, Height>, Width>;
	struct Params{
		size_t max_branch, branch_rate, max_size;
	} branch_param;//parameter pack
	_2dArray<H/2*2, W/2*2, bool> bBody;//main parameter
	_2dArray<H/2*2, W/2*2, bool> escape;
	std::multimap<size_t, size_t> escape_map;
public:
	Labyrinth(Exist ex = Exist::HORIZONTAL, size_t max_branch = 100, size_t branch_rate = 1000, size_t branch_size = 50) ://standart constructor
		branch_param{ max_branch, branch_rate, branch_size }
	{
		build_frames();
		build_path(ex);
		build_subpath(ex);
	}
	void regenerate(Exist ex = Exist::VERTICAL);//generation new labyrinth with old pamametrs
	const auto& get() const //returns an object in a bool array performance
	{
		return bBody;
	} 
	const auto& path() const //return exit path in a bool array performance
	{
		return escape;
	}
	const bool& at(size_t X, size_t Y) const
	{
		return bBody[Y][X];
	}
	bool& at(size_t X, size_t Y)
	{
		return bBody[Y][X];
	}
private:
	void build_path(Exist);
	void build_frames();
	void init_map()
	{
		for (size_t y = 2; y != H / 2 * 2 - 2; ++y)
			for (size_t x = 2; x != W / 2 * 2 - 2; ++x)
				if (!escape[y][x])
					escape_map.insert(std::pair<const size_t, size_t>{y, x});
	}
	void build_subpath(Exist ex)
	{
		static std::default_random_engine e(static_cast<unsigned>(time(0)));
		std::bernoulli_distribution ber(branch_param.branch_rate / 1000.);
		std::vector<std::multimap<size_t, size_t>::value_type> sample_map{ (H * W) / (H + W) };
		std::sample(escape_map.cbegin(), escape_map.cend(), std::back_inserter(sample_map), (H * W) / (H + W), std::mt19937(std::random_device{}()));
#ifndef _FAST_BUILD_
		std::vector<std::pair<size_t, size_t>> ends{ (H * W) / (H + W) };
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
				ends.push_back(subpath(i.second, i.first, ex));
#endif // !_FAST_BUILD_
			}
		}
		//step 2
#ifndef _FAST_BUILD_
		std::shuffle(ends.begin(), ends.end(), std::mt19937(std::random_device{}()));
		for (const auto& [x, y] : ends)
		{
			subpath(x, y, branch_param.max_branch);
		}
#endif // !_FAST_BUILD_
		//step 3 (final)
		if (ex == Exist::HORIZONTAL)
			for (size_t x = 3; x != bBody[0].size() - 3; ++x)
			{
				for (size_t y = 3; y != bBody.size() - 3; ++y)
				{
					if (!bBody[y][x] && ber(e) && x % 2 == 0 && y % 2 == 0)
						subpath(x, y, branch_param.max_branch);
				}
			}
		else
			for (size_t y = 3; y != bBody.size() - 3; ++y)
			{
				for (size_t x = 3; x != bBody[0].size() - 3; ++x)
				{
					if (!bBody[y][x] && ber(e) && x % 2 == 0 && y % 2 == 0)
						subpath(x, y, branch_param.max_branch);
				}
			}
	}
	std::pair<size_t, size_t> subpath(size_t x, size_t y, Exist ex)//for step 1
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
			d_x.param(std::uniform_int_distribution<>::param_type{ -1,1 });
			if (d(e))
				d_y.param(std::uniform_int_distribution<>::param_type{ 0,1 });
			else
				d_y.param(std::uniform_int_distribution<>::param_type{ -1,0 });
		}
		else
		{
			d_y.param(std::uniform_int_distribution<>::param_type{ -1,1 });
			if (d(e))
				d_x.param(std::uniform_int_distribution<>::param_type{ 0,1 });
			else
				d_x.param(std::uniform_int_distribution<>::param_type{ -1,0 });
		}
		while (counter < branch_param.max_size && flag)
		{
			proc = d_x(e);
			delta = d_y(e);
			if (proc && x + proc * STEP < W / 2 * 2 - 3 && x + proc * STEP > 3 && bBody[y][x + proc * STEP] && bBody[y - 1][x + proc * STEP] && bBody[y + 1][x + proc * STEP] && bBody[y][x + proc * (STEP + 1)] && bBody[y][x + proc])
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
			if (delta && y + delta * STEP > Y_LIMIT && y + delta * STEP < H / 2 * 2 - 2 && bBody[delta * STEP + y][x] && bBody[y + delta * STEP][x + 1] && bBody[y + delta * STEP][x - 1] && bBody[y + delta * (STEP + 1)][x] && bBody[y + delta][x])
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
	void subpath(size_t x, size_t y, size_t re_count)
	{
		static std::default_random_engine e(static_cast<unsigned>(time(0)));
		std::uniform_int_distribution<> d(-1, 1);
		std::bernoulli_distribution ber(branch_param.branch_rate/1000.);
		uint8_t counter = 0;
		uint8_t status = 0;
		bool flag = true;
		int proc = d(e), delta = d(e);
		while (counter < branch_param.max_size && flag)
		{
			proc = d(e), delta = d(e);
			if (proc && x + proc * STEP < W / 2 * 2 - 3 && x + proc * STEP > 3 && bBody[y][x + proc * STEP] && bBody[y - 1][x + proc * STEP] && bBody[y + 1][x + proc * STEP] && bBody[y][x + proc * (STEP + 1)] && bBody[y][x + proc])
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
			if (delta && y + delta * STEP > Y_LIMIT && y + delta * STEP < H / 2 * 2 - 2 && bBody[delta * STEP + y][x] && bBody[y + delta * STEP][x + 1] && bBody[y + delta * STEP][x - 1] && bBody[y + delta * (STEP + 1)][x] && bBody[y + delta][x])
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
	bool hor_check(size_t indx, size_t indy, short course = 1) const
	{
		for (; indx != W / 2 + W / 2 * course; indx += course)
			if (!bBody[indy][indx])
				return true;
		return false;
	}
	bool ver_check(size_t indx, size_t indy, short course = 1) const
	{
		for (; indy != H / 2 + H / 2 * course; indy += course)
			if (!bBody[indy][indx])
				return true;
		return false;
	}
};

template <unsigned W, unsigned H>
void Labyrinth<W, H>::build_frames()
{
	for (auto& y : bBody)
		for (auto& x : y)
			x = true;
	for (auto& y : escape)
		for (auto& x : y)
			x = true;
}

template <unsigned W, unsigned H>
void Labyrinth<W, H>::build_path(Exist ex)
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
		std::bernoulli_distribution cf(std::clamp(1.56 - pow(static_cast<double>(H) / W, COEFFICIENT), 0.1, 1.));//cf - curvature factor
		while (flag) 
		{
			x = 0, y = ((H / 2) % 2) ? H / 2 + 1 : H / 2;
			escape[y][x + 1] = bBody[y][x + 1] = escape[y][x] = bBody[y][x] =
				escape[y][x + 2] = bBody[y][x + 2] = false;
			x += 2;
			while (x < W / 2 * 2 - 2 && counter != W * H  )
			{
				++counter;
				status = proc(e);
				dx = delta(e);
				if (dx == -1 && cf(e) || counter < 6)
					dx = 1;
				dy = delta(e);
				if (status && dx && x + dx * 2 > 2 && x + dx * 2 < W / 2 * 2 &&
					escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2])
				{
					x += dx * 2;
					escape[y][x - dx] = bBody[y][x - dx] =
						escape[y][x] = bBody[y][x] = false;
					if (proc(e) && x + dx * 2 > 2 && x + dx * 2 < W / 2 * 2 &&
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
					if (x + dx * 2 > 2 && x + dx * 2 < W / 2 * 2 &&
						escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2] &&
						escape[y - 1][x + dx * 2])
					{
						x += dx * 2;
						escape[y][x - dx] = bBody[y][x - dx] =
							escape[y][x] = bBody[y][x] = false;
						if (proc(e) && x + dx * 2 > 2 && x + dx * 2 < W / 2 * 2 &&
							escape[y][x + dx * 2] && escape[y - 1][x + dx * 2] && escape[y + 1][x + dx * 2] &&
							escape[y - 1][x + dx * 2])
						{

							x += dx * 2;
							escape[y][x - dx] = bBody[y][x - dx] =
								escape[y][x] = bBody[y][x] = false;
						}
					}
				}

				if (dy && y + dy * STEP > 2 && y + dy * STEP < H / 2 * 2 - 2 && escape[y + dy * STEP][x] &&
					escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
				{
					y += dy * STEP;
					escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
					if (!cf(e) && y + dy * STEP > 2 && y + dy * STEP < H / 2 * 2 - 2 && escape[y + dy * STEP][x] &&
						escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
					{
						y += dy * STEP;
						escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
					}
				}
				else if (dy)
				{
					dy = -dy;
					if (y + dy * STEP > 2 && y + dy * STEP < H / 2 * 2 - 2 && escape[y + dy * STEP][x] &&
						escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
					{
						y += dy * STEP;
						escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
						if (!cf(e) && y + dy * STEP > 2 && y + dy * STEP < H / 2 * 2 - 2 && escape[y + dy * STEP][x] &&
							escape[y + dy * (STEP + 1)][x] && !hor_check(x, y + dy * STEP))
						{
							y += dy * STEP;
							escape[y - dy][x] = bBody[y - dy][x] = escape[y][x] = bBody[y][x] = false;
						}
					}
				}
			}
			if (counter != W * H)
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
		std::bernoulli_distribution cf(std::clamp(1.56 - pow(static_cast<double>(W) / H, COEFFICIENT), 0.1, 1.));
		while (flag)
		{
			x = ((W / 2) % 2) ? W / 2 + 1 : W / 2, y = 0;
			escape[y + 1][x] = bBody[y + 1][x] = escape[y][x] = bBody[y][x] =
				escape[y + 2][x] = bBody[y + 2][x] = false;
			y += 2;
			while (y < H / 2 * 2 - 2 && counter != W * H)
			{
				++counter;
				status = proc(e);
				dy = delta(e);
				if (dy == -1 && cf(e) || counter < 3)
					dy = 1;
				dx = delta(e);
				if (status && dy && y + dy * 2 > 1 && y + dy * 2 < H / 2 * 2 &&
					escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
				{
					y += dy * 2;
					escape[y - dy][x] = bBody[y - dy][x] =
						escape[y][x] = bBody[y][x] = false;
					if (proc(e) && y + dy * 2 > 1 && y + dy * 2 < H / 2 * 2 &&
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
					if (y + dy * 2 > 1 && y + dy * 2 < H / 2 * 2  &&
						escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
					{
						y += dy * 2;
						escape[y - dy][x] = bBody[y - dy][x] =
							escape[y][x] = bBody[y][x] = false;
						if (proc(e) && y + dy * 2 > 1 && y + dy * 2 < H / 2 * 2 &&
							escape[y + dy * 2][x] && escape[y + dy * 2][x - 1] && escape[y + dy * 2][x + 1])
						{
							y += dy * 2;
							escape[y - dy][x] = bBody[y - dy][x] =
								escape[y][x] = bBody[y][x] = false;
						}
					}
				}
				if (dx && x + dx * STEP > 2 && x + dx * STEP < W / 2 * 2 - 3 && escape[y][x + dx * STEP] &&
					escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
				{
					x += dx * STEP;
					escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
					if (!cf(e) && x + dx * STEP > 2 && x + dx * STEP < W / 2 * 2 - 3 && escape[y][x + dx * STEP] &&
						escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
					{
						x += dx * STEP;
						escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
					}
				}
				else if (dx)
				{
					dx = -dx;
					if (x + dx * STEP > 2 && x + dx * STEP < W / 2 * 2 - 3 && escape[y][x + dx * STEP] &&
						escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
					{
						x += dx * STEP;
						escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
						if (!cf(e) && x + dx * STEP > 2 && x + dx * STEP < W / 2 * 2 - 3 && escape[y][x + dx * STEP] &&
							escape[y][x + dx * (STEP + 1)] && !ver_check(x + dx * STEP, y))
						{
							x += dx * STEP;
							escape[y][x - dx] = bBody[y][x - dx] = escape[y][x] = bBody[y][x] = false;
						}
					}
				}
			}
			if (counter != W * H)
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

template <unsigned W, unsigned H>
void Labyrinth<W, H>::regenerate(Exist ex)
{
	build_frames();
	build_path(ex);
	build_subpath();
}