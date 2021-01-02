/*
* Header containing Labyrinth generator(vector version)
* version 2.0.0
* Author: Damir Hismatov
* Github: https://github.com/Daedma
*/
#pragma once
#include <iosfwd>
#include <map>
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

class Labyrinth//Class for random generation of labyrinth
{
    template<typename T>
    using _2dArray = std::vector<std::vector<T>>;
    struct Params
    {
        size_t max_branch, branch_rate, max_size;
    } branch_param;//parameter pack
    _2dArray<bool> bBody;//main parameter
    _2dArray<bool> escape;
    size_t width, height;
    std::multimap<size_t, size_t> escape_map;
    public:
    Labyrinth(size_t = 100, size_t = 100, Exist = Exist::HORIZONTAL, size_t = 100, size_t = 1000, size_t = 50);
    void regenerate(Exist ex = Exist::VERTICAL);//generation new labyrinth with old pamametrs
    const auto& get() const //returns an object in a std::vector<bool> performance
    {
        return bBody;
    }
    const auto& path() const//return exit path in a std::vector<bool> performance
    {
        return escape;
    }
    const auto at(size_t X, size_t Y) const
    {
        return bBody.at(Y).at(X);
    }
    auto at(size_t X, size_t Y)
    {
        return bBody.at(Y).at(X);
    }
    private:
    void build_path(Exist);
    void build_frames();
    void init_map();
    void build_subpath(Exist);
    std::pair<size_t, size_t> subpath(size_t, size_t, Exist);//for step 1
    void subpath(size_t, size_t, size_t);
    bool hor_check(size_t, size_t, short = 1) const;
    bool ver_check(size_t, size_t, short = 1) const;
};

/*
* default displays Labyrinth and path separately
* use macros _PRINT_WITHOUT_EXIT_ , if you want print labyrinth without exit path separately
* use macros _PRINT_WITH_EXIT_ , if you want highlight exit inside labyrinth
*/
std::ostream& operator<<(std::ostream&, const Labyrinth&);