
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

#endif //GS_MATTERHORN_MATHUTILS_H
