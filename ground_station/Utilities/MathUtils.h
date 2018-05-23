
#ifndef GS_MATTERHORN_MATHUTILS_H
#define GS_MATTERHORN_MATHUTILS_H

#include <algorithm>

template<class T>
constexpr const T &
clamp(const T &v, const T &lo, const T &hi) {
    return std::min(
            std::max(v, lo),
            hi
    );
}

template<class T>
T
normalize(const T &v, const T &a, const T &b) {
    return (clamp(v, a, b) - a) / (b - a);
}

template<class T>
T
lerp(const T &a, const T &b, const float &v) {
    return (1.0f - v) * a + v * b;
}

#endif //GS_MATTERHORN_MATHUTILS_H
