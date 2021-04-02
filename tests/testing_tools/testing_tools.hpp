#include <sstream>
#include <mutex>
#include <chrono>
#include <type_traits>

namespace test{

    //Class for concurrent output
    struct pcout : public std::stringstream
    {
        ~pcout();
        pcout(const std::string&);
        pcout() = default;
    private:
        std::string file;
        static inline std::mutex cout_mutex;
        static inline std::mutex ofs_mutex;
    };

    //Class-timer
    struct timer
    {
    private:
        using clock_t = std::chrono::high_resolution_clock;
        using second_t = std::chrono::duration<double, std::ratio<1> >;

        std::chrono::time_point<clock_t> m_beg;

    public:
        timer() : m_beg(clock_t::now())
        {}

        void reset()
        {
            m_beg = clock_t::now();
        }

        double elapsed() const
        {
            return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
        }
    };

    //Basic functions for testing dislubability
    template <typename Object, typename ...Args>
    double time_for_construct(Args&&... args)
    {
        test::timer t;
        Object { std::forward<Args>(args)... };
        return t.elapsed();
    }

    template <typename Func, typename ...Args>
    auto averaged_result(size_t _Iteration, Func _Called, Args&&... args)
    {
        double sum { 0 };
        for (auto i = 0; i != _Iteration; ++i)
        {
            sum += _Called(std::forward<Args>(args)...);
        }
        return sum / static_cast<double>(_Iteration);
    }
}