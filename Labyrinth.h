#pragma once
#include <array>
#include <iostream>
#include <random>
#include <map>
#include <ctime>

#define BLOCK char(219)
#define STEP 2
#define STATUS_CUP 4
#define Y_LIMIT 1

enum class Exist
{
	VERTICAL, HORIZONTAL
};

template <unsigned W, unsigned H>//width and height
class Labyrinth
{
	friend std::ostream& operator<<(std::ostream& os, const Labyrinth<W, H>& lab)
	{
		for (const auto& y : lab.bBody)
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
		std::cout << std::endl;
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
		return os;
	}
	template<unsigned Width, unsigned Height, typename T>
	using _2dArray = std::array<std::array<T, Height>, Width>;
	struct Params
	{
		size_t max_branch, branch_rate, max_size;
	} branch_param;
	_2dArray<H/2*2, W/2*2, bool> bBody;
	_2dArray<H/2*2, W/2*2, bool> escape;
	static std::multimap<size_t, size_t> escape_map;
public:
	Labyrinth(Exist ex = Exist::HORIZONTAL, size_t max_branch = 10, size_t branch_rate = 600, size_t branch_size = 50) ://standart constructor
		branch_param{ max_branch, branch_rate, branch_size }
	{
		build_frames();
		build_path(ex);
		build_subpath();
	}
	void regenerate(Exist ex = Exist::VERTICAL);//generation new labyrinth
	const auto& get() const //returns an object in a bool array performance
	{
		return bBody;
	} 
	const auto& path() const //return true path in a bool array performance
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
		for (auto y = 0; y != escape.size(); ++y)
			for (auto x = 0; x != escape[y].size(); ++x)
				if (!escape[y][x] && y > 1 && y < H / 2 * 2 - 2 && x>1 && x < W / 2 * 2 - 2)
					escape_map.insert(std::make_pair(y, x));
	}
	void build_subpath()
	{
		static std::default_random_engine e(static_cast<unsigned>(time(0)));
		std::bernoulli_distribution ber(branch_param.branch_rate / 1000.);
		for (auto& i : escape_map)
		{
			if (i.second % 2 == 0 && ber(e))
				subpath(i.second, i.first, branch_param.max_branch);
		}
	}
	void subpath(size_t x, size_t y, size_t re_count)
	{
		static std::default_random_engine e(static_cast<unsigned>(time(0)));
		std::uniform_int_distribution<> d(-1, 1);
		std::bernoulli_distribution ber(branch_param.branch_rate/1000.);
		uint8_t counter = 0;
		static uint8_t status = 0;
		bool flag = true;
			while (counter != branch_param.max_size && flag)
			{
				++counter;
				int dx = d(e), dy = d(e);
				if (dx && x + dx * STEP < W / 2 * 2 - 3 && x + dx * STEP > 3 && bBody[y][x + dx * STEP] && bBody[y - 1][x + dx * STEP] && bBody[y + 1][x + dx * STEP] && bBody[y][x + dx * (STEP + 1)] && bBody[y][x + dx])
				{
					x += dx * STEP;
					status = 0;
					bBody[y][x - dx] = bBody[y][x] = false;
				}
				else if(dx)
				{
					++status;
					if (status == STATUS_CUP) flag = false;
				}
				if (dy && y + dy * STEP > Y_LIMIT && y + dy * STEP < H / 2 * 2 - 2 && bBody[dy * STEP + y][x] && bBody[y + dy * STEP][x + 1] && bBody[y + dy * STEP][x - 1] && bBody[y + dy * (STEP + 1)][x] && bBody[y + dy][x])
				{
					y += dy * STEP;
					status = 0;
					bBody[y - dy][x] = bBody[y][x] = false;
				}
				else if(dy)
				{
					++status;
					if (status == STATUS_CUP) flag = false;
				}
				if (ber(e) && re_count && flag)
				{
					subpath(x, y, re_count - 1);
				}
			}
		status = 0;
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
std::multimap<size_t, size_t> Labyrinth<W, H>::escape_map;

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
	static std::default_random_engine e(static_cast<size_t>(time(0)));
	if (ex == Exist::HORIZONTAL)
	{
		std::uniform_int_distribution<> dy(-1, 1);
		std::bernoulli_distribution dx(0.9);
		std::bernoulli_distribution ch(0.5);
		size_t counter = 0;
		bool flag = true;
		size_t bx = 0, by = H / 2;
		while (flag) 
		{
			bx = 0;
			by = H / 2;
			escape[by][bx + 1] = bBody[by][bx + 1] = escape[by][bx] = bBody[by][bx] =
				escape[by][bx + 2] = bBody[by][bx + 2] = false;
			bx += 2;
			while (bx < W / 2 * 2 - 2 && counter != W + H  )
			{
				++counter;
				bool status = dx(e);
				auto nx = dy(e);
				if (nx == -1 && ch(e) || counter < 6)
					nx = 1;
				auto ny = dy(e);
				if (status && nx && bx + nx * 2 > 2 && bx + nx * 2 < W / 2 * 2 &&
					escape[by][bx + nx * 2] && escape[by - 1][bx + nx * 2] && escape[by + 1][bx + nx * 2])
				{
					bx += nx * 2;
					escape[by][bx - nx] = bBody[by][bx - nx] =
						escape[by][bx] = bBody[by][bx] = false;
					if (dx(e) && bx + nx * 2 > 2 && bx + nx * 2 < W / 2 * 2 &&
						escape[by][bx + nx * 2] && escape[by - 1][bx + nx * 2] && escape[by + 1][bx + nx * 2])
					{

						bx += nx * 2;
						escape[by][bx - nx] = bBody[by][bx - nx] =
							escape[by][bx] = bBody[by][bx] = false;
					}
				}
				else if (nx)
				{
					nx = -nx;
					if (bx + nx * 2 > 2 && bx + nx * 2 < W / 2 * 2 &&
						escape[by][bx + nx * 2] && escape[by - 1][bx + nx * 2] && escape[by + 1][bx + nx * 2] &&
						escape[by - 1][bx + nx * 2])
					{
						bx += nx * 2;
						escape[by][bx - nx] = bBody[by][bx - nx] =
							escape[by][bx] = bBody[by][bx] = false;
						if (dx(e) && bx + nx * 2 > 2 && bx + nx * 2 < W / 2 * 2 &&
							escape[by][bx + nx * 2] && escape[by - 1][bx + nx * 2] && escape[by + 1][bx + nx * 2] &&
							escape[by - 1][bx + nx * 2])
						{

							bx += nx * 2;
							escape[by][bx - nx] = bBody[by][bx - nx] =
								escape[by][bx] = bBody[by][bx] = false;
						}
					}
				}

				if (ny && by + ny * STEP > 2 && by + ny * STEP < H / 2 * 2 - 2 && escape[by + ny * STEP][bx] &&
					escape[by + ny * (STEP + 1)][bx] && !hor_check(bx, by + ny * STEP))
				{
					by += ny * STEP;
					escape[by - ny][bx] = bBody[by - ny][bx] = escape[by][bx] = bBody[by][bx] = false;
					if (dx(e) && by + ny * STEP > 2 && by + ny * STEP < H / 2 * 2 - 2 && escape[by + ny * STEP][bx] &&
						escape[by + ny * (STEP + 1)][bx] && !hor_check(bx, by + ny * STEP))
					{
						by += ny * STEP;
						escape[by - ny][bx] = bBody[by - ny][bx] = escape[by][bx] = bBody[by][bx] = false;
					}
				}
				else if (ny)
				{
					ny = -ny;
					if (by + ny * STEP > 2 && by + ny * STEP < H / 2 * 2 - 2 && escape[by + ny * STEP][bx] &&
						escape[by + ny * (STEP + 1)][bx] && !hor_check(bx, by + ny * STEP))
					{
						by += ny * STEP;
						escape[by - ny][bx] = bBody[by - ny][bx] = escape[by][bx] = bBody[by][bx] = false;
						if (dx(e) && by + ny * STEP > 2 && by + ny * STEP < H / 2 * 2 - 2 && escape[by + ny * STEP][bx] &&
							escape[by + ny * (STEP + 1)][bx] && !hor_check(bx, by + ny * STEP))
						{
							by += ny * STEP;
							escape[by - ny][bx] = bBody[by - ny][bx] = escape[by][bx] = bBody[by][bx] = false;
						}
					}
				}
			}
			if (counter != W + H)
			{
				init_map();
				flag = false;
			}
			else
			{
				build_frames();
				counter = 0;
			}
		}
		escape[by][bx + 1] = bBody[by][bx + 1] = false;
	}
	else
	{
		std::uniform_int_distribution<> dy(-1, 1);
		std::bernoulli_distribution dx(0.6);
		std::bernoulli_distribution ch(0.4);
		size_t counter = 0;
		bool flag = true;
		size_t bx = W/2, by = 0;
		while (flag)
		{
			bx = W/2;
			by = 0;
			escape[by + 1][bx] = bBody[by + 1][bx] = escape[by][bx] = bBody[by][bx] =
				escape[by + 2][bx] = bBody[by + 2][bx] = false;
			by += 2;
			while (by < H / 2 * 2 - 2 && counter != W + H)
			{
				++counter;
				bool status = dx(e);
				auto ny = dy(e);
				if (ny == -1 && ch(e) || counter < 3)
					ny = 1;
				auto nx = dy(e);
				if (status && ny && by + ny * 2 > 1 && by + ny * 2 < H / 2 * 2 &&
					escape[by + ny * 2][bx] && escape[by + ny * 2][bx - 1] && escape[by + ny * 2][bx + 1])
				{
					by += ny * 2;
					escape[by - ny][bx] = bBody[by - ny][bx] =
						escape[by][bx] = bBody[by][bx] = false;
					if (dx(e) && by + ny * 2 > 1 && by + ny * 2 < H / 2 * 2 &&
						escape[by + ny * 2][bx] && escape[by + ny * 2][bx - 1] && escape[by + ny * 2][bx + 1])
					{
						by += ny * 2;
						escape[by - ny][bx] = bBody[by - ny][bx] =
							escape[by][bx] = bBody[by][bx] = false;
					}
				}
				else if (ny)
				{
					ny = -ny;
					if (by + ny * 2 > 1 && by + ny * 2 < H / 2 * 2  &&
						escape[by + ny * 2][bx] && escape[by + ny * 2][bx - 1] && escape[by + ny * 2][bx + 1])
					{
						by += ny * 2;
						escape[by - ny][bx] = bBody[by - ny][bx] =
							escape[by][bx] = bBody[by][bx] = false;
						if (dx(e) && by + ny * 2 > 1 && by + ny * 2 < H / 2 * 2 &&
							escape[by + ny * 2][bx] && escape[by + ny * 2][bx - 1] && escape[by + ny * 2][bx + 1])
						{
							by += ny * 2;
							escape[by - ny][bx] = bBody[by - ny][bx] =
								escape[by][bx] = bBody[by][bx] = false;
						}
					}
				}
				if (nx && bx + nx * STEP > 2 && bx + nx * STEP < W / 2 * 2 - 3 && escape[by][bx + nx * STEP] &&
					escape[by][bx + nx * (STEP + 1)] && !ver_check(bx + nx * STEP, by))
				{
					bx += nx * STEP;
					escape[by][bx - nx] = bBody[by][bx - nx] = escape[by][bx] = bBody[by][bx] = false;
					if (dx(e) && bx + nx * STEP > 2 && bx + nx * STEP < W / 2 * 2 - 3 && escape[by][bx + nx * STEP] &&
						escape[by][bx + nx * (STEP + 1)] && !ver_check(bx + nx * STEP, by))
					{
						bx += nx * STEP;
						escape[by][bx - nx] = bBody[by][bx - nx] = escape[by][bx] = bBody[by][bx] = false;
					}
				}
				else if (nx)
				{
					nx = -nx;
					if (bx + nx * STEP > 2 && bx + nx * STEP < W / 2 * 2 - 3 && escape[by][bx + nx * STEP] &&
						escape[by][bx + nx * (STEP + 1)] && !ver_check(bx + nx * STEP, by))
					{
						bx += nx * STEP;
						escape[by][bx - nx] = bBody[by][bx - nx] = escape[by][bx] = bBody[by][bx] = false;
						if (dx(e) && bx + nx * STEP > 2 && bx + nx * STEP < W / 2 * 2 - 3 && escape[by][bx + nx * STEP] &&
							escape[by][bx + nx * (STEP + 1)] && !ver_check(bx + nx * STEP, by))
						{
							bx += nx * STEP;
							escape[by][bx - nx] = bBody[by][bx - nx] = escape[by][bx] = bBody[by][bx] = false;
						}
					}
				}
			}
			if (counter != W + H)
			{
				init_map();
				flag = false;
			}
			else
			{
				build_frames();
				counter = 0;
			}
		}
		escape[by + 1][bx] = bBody[by + 1][bx] = false;
	}
}

template <unsigned W, unsigned H>
void Labyrinth<W, H>::regenerate(Exist ex)
{
	build_frames();
	build_path(ex);
	build_subpath();
}