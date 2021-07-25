/*
* Header containing Labyrinth generator(vector version)
* version 2.2.1
* Author: Damir Hismatov
* Github: https://github.com/Daedma
*/
#pragma once
#include <iosfwd>
#include <map>
#include <vector>
#include <random>
#include <string>

#define WIDTH_LIMIT 16//smallest acceptable width
#define HEIGHT_LIMIT 16//smallest acceptable height

/*
* format for a string describing the maze generation:
* the middle three letters - the value exist (Labyrinth :: exist), can have the value hor or ver;
* following the exist value, the unsigned number is the seed, read to point;
* two numbers of unsigned type and separated by 'x' after the separating dot are
* values for width and height, have values greater than WIDTH_LIMIT and HEIGHT_LIMIT, respectively.
* =========================================
* any deviation from the format throws an exception of type std :: invalid_argument,
* in this case, the labyrinth will not be generated and have an undefined state (security guarantees for exceptions are not observed)
* =========================================
* Example: Labyrinth{"hor1234567890.1000x1000"} equal Labyrinth{Labyrinth::exist::hor, 1234567890, 1000, 1000}
*/

/*
* the width and height values may not match those provided by the user,
* since there is getting rid of parity upward
* (that is, if the user provided an even value for i, then the value i + 1 will be used)
*/
class Labyrinth final//Class for random generation of labyrinth
{
public:

    using seed_type = uint64_t;
    /*
    * ver - means building a labyrinth with exits on the sides
    * hor - means building a labyrinth with exits from above and below
    */
    enum class exist//parameters to pass to constructor and function regeneration
    {
        ver, //exits on the sides
        hor //exits from above and below
    };
    Labyrinth(size_t, size_t, exist, size_t, size_t, size_t, seed_type = std::random_device {}());//full control over all parameters (not recommended)
    Labyrinth(exist, seed_type, size_t, size_t);//exist, seed, width, height
    Labyrinth(exist, size_t, size_t);//exist, width, height
    explicit Labyrinth(const std::string&);//seed
    const seed_type& regenerate();//generate new labyrinth with new seed and old width, height, exist
    const seed_type& regenerate(const std::string&);//generate new labyrinth
    const seed_type& regenerate(exist, seed_type, size_t, size_t);//generate new labyrinth
    const seed_type& regenerate(exist, size_t, size_t);//generate new labyrinth
    void regenerate(exist, size_t, size_t, size_t, size_t, size_t, seed_type = std::random_device {}());//full control over all parameters (not recommended)
    const seed_type& seed() const//get seed
    {
        return gen_key;
    }
    void swap(Labyrinth&) noexcept;
    const std::string seed_s() const;//get seed in string perfomance
    const auto& get() const //returns an object in a two-dimensional std::vector<bool> performance
    {
        return bBody;
    }
    const auto& path() const//return exit path in a two-dimensional std::vector<bool> performance
    {
        return escape;
    }
    const std::pair<size_t, size_t>  entry() const;//entry coordinates
    const std::pair<size_t, size_t> exit() const;//exit coordinates
    const auto at(size_t X, size_t Y) const
    {
        return bBody.at(Y).at(X);
    }
    auto at(size_t X, size_t Y)
    {
        return bBody.at(Y).at(X);
    }

private:

    template<typename T>
    using _2dArray = std::vector<std::vector<T>>;
    //members
    struct
    {
        size_t max_branch, branch_rate, max_size;
    } branch_param;//parameter pack // c-style be like ^^
    size_t width, height;
    _2dArray<bool> bBody;//main parameter describing the labyrinth
    _2dArray<bool> escape;//exit map in a bool array perfomance
    seed_type gen_key;//generation key
    std::default_random_engine engine;//random engine
    //functions
    exist init(const std::string&);//initialization of labyrinth parameters via string
    inline exist spot_ex() const;//spot labyrinth type (exist :: ver or exist :: hor)
    void init_branch_param();//initializing branch_param based on width and height
    std::multimap<size_t, size_t> init_map();//makes a map of the exit route
    void reset();//resets path and maze to initial state (sets bits to escape and bBody to true)
    void build_path(exist);//creates an exit path
    void build_subpath(exist);//full filling of the labyrinth with branches
    std::pair<size_t, size_t> subpath(int64_t, int64_t, exist);//for step 1 in building branches from path
    void subpath(int64_t, int64_t, size_t);//for building branches
    bool hor_check(size_t, size_t, size_t, short = 1) const;//function to check for cross paths when building along X-coordinate
    bool ver_check(size_t, size_t, size_t, short = 1) const;//function to check for cross paths when building along Y-coordinate
    bool h_impasse(size_t, size_t, size_t) const;//function to avoid "getting stuck" between two parallel paths when building a path along the X-coordinate
    bool v_impasse(size_t, size_t, size_t) const;//function to avoid "getting stuck" between two parallel paths when building a path along the Y-coordinate
};

void swap(Labyrinth&, Labyrinth&) noexcept;

/*
* default displays Labyrinth and path separately
* use macros _PRINT_WITHOUT_EXIT_ , if you want print labyrinth without exit path separately (not working)
* use macros _PRINT_WITH_EXIT_ , if you want highlight exit inside labyrinth (not working)
*/
std::ostream& operator<<(std::ostream&, const Labyrinth&);