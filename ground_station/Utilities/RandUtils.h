#ifndef GS_MATTERHORN_RANDUTILS_H
#define GS_MATTERHORN_RANDUTILS_H

#include <random>
#include <cfloat>
#include <functional>
#include <type_traits>
#include <typeinfo>

// Adapted from https://stackoverflow.com/questions/22895130/creating-full-range-of-random-floats-using-stdrandom
// and https://stackoverflow.com/questions/32907254/uniform-random-distribution-base-class-for-both-int-and-double

/**
 * Specify the distribution type depending on the type of the parameter type T.
 */
template<typename T>
using distributionType = std::conditional_t<std::is_integral<T>::value,
        std::uniform_int_distribution<T>,
        std::uniform_real_distribution<T>>;

/**
 * A class generating uniformly distributed random numbers whose types match the parameter type T.
 * @tparam T
 */
template<typename T>
class Rand {
public:
    explicit Rand(T lo = std::numeric_limits<T>::min(),
                  T hi = std::numeric_limits<T>::max()) :
            r(bind(distributionType<T>(lo, hi), std::mt19937{})) {}

    T operator()() const { return r(); }

private:
    std::function<T()> r;
};

#endif