
#ifndef GS_MATTERHORN_FACTORIES_H
#define GS_MATTERHORN_FACTORIES_H


#include <DataStructures/datastructs.h>
#include <vector>

static_assert(sizeof(float) == 4,
              "In order for the decoder to be able to cast floats to uint32 safely, sizeof(float) should return 4");

typedef union {
    float fl;
    uint32_t uint32;
} float_cast;

class PayloadDataConverter {

public:
    static SensorsPacket toSensorsPacket(const std::vector<uint8_t> &);

    static EventPacket toEventPacket(const std::vector<uint8_t> &);

    static ControlPacket toControlPacket(const std::vector<uint8_t> &);

    static GPSPacket toGPSPacket(const vector<uint8_t> &);
};


#endif //GS_MATTERHORN_FACTORIES_H
