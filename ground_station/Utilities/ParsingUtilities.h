#ifndef GS_MATTERHORN_PARSINGUTILITIES_H
#define GS_MATTERHORN_PARSINGUTILITIES_H

static uint16_t parseUint16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

static int16_t parseInt16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

static uint32_t parseUint32(vector<uint8_t>::iterator &it) {
    return (*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}

#endif //GS_MATTERHORN_PARSINGUTILITIES_H
