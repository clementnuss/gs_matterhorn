
#ifndef GS_MATTERHORN_TIMEUTILS_H
#define GS_MATTERHORN_TIMEUTILS_H

#include <chrono>

static long long msecsBetween(std::chrono::system_clock::time_point t1, std::chrono::system_clock::time_point t2) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
}

static long long usecsBetween(std::chrono::system_clock::time_point t1, std::chrono::system_clock::time_point t2) {
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
}

#endif //GS_MATTERHORN_TIMEUTILS_H
