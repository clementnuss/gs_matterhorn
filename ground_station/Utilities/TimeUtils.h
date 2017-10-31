
#ifndef GS_MATTERHORN_TIMEUTILS_H
#define GS_MATTERHORN_TIMEUTILS_H

static long long msecsBetween(chrono::system_clock::time_point t1, chrono::system_clock::time_point t2) {
    return chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
}

#endif //GS_MATTERHORN_TIMEUTILS_H
