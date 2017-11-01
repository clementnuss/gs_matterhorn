#ifndef GS_MATTERHORN_PARSINGUTILITIES_H
#define GS_MATTERHORN_PARSINGUTILITIES_H


template<typename T>
static T parse16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

template<typename T>
static T parse32(vector<uint8_t>::iterator &it) {
    return (*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}

#endif //GS_MATTERHORN_PARSINGUTILITIES_H
