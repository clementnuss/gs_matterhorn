
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

class Factories {

public:
    static shared_ptr<IDeserializable> telemetryReadingFactory(std::vector<uint8_t>);
};


#endif //GS_MATTERHORN_FACTORIES_H
