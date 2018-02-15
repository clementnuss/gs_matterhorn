
#ifndef GS_MATTERHORN_PAYLOADTYPE_H
#define GS_MATTERHORN_PAYLOADTYPE_H


#include <DataStructures/datastructs.h>
#include <memory>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>

/**
 * PayloadType is a Java-like enum which for a given PayloadType encapsulates its length as
 * well as a pointer to a function which knows how to decode its contents. The length is used
 * by the decoder to accumulate the right number of bytes before calling the ecapsulated
 * decoding function with the bytes accumulated.
 */
class PayloadType {
public:
    static const PayloadType TELEMETRY;
    static const PayloadType EVENT;
    static const PayloadType CONTROL;
    static const PayloadType GPS;

    static const std::map<int, PayloadType> TYPES_TABLE;

private:
    const int code_;
    const size_t length_;

    PayloadType(int code, size_t length) :
            code_{code}, length_{length} {}

    static std::map<int, PayloadType> createPayloadTypesMap() {
        return std::map<int, PayloadType>{
                {CommunicationsConstants::TELEMETRY_TYPE, TELEMETRY},
                {CommunicationsConstants::EVENT_TYPE,     EVENT},
                {CommunicationsConstants::CONTROL_TYPE,   CONTROL},
                {CommunicationsConstants::GPS_TYPE,       GPS}
        };
    }

public:
    PayloadType &operator=(const PayloadType &) = default;

    /**
     * Int operator overriding allows to use a PayloadType class in
     * switch statements
     *
     * @return
     */
    operator int() const {
        return code_;
    }

    /**
     *
     * @return The length associated with this payload type.
     */
    size_t length() const {
        return length_;
    }

    /**
     * Converts a code number into one of the different PayloadType enum values
     * If the code is not used, an assertion is triggered as this indicates an
     * error in the processing or sending of datagrams and is not a correct situation
     *
     * @param code The code to be converted to a PayloadType
     * @return The PayloadType corresponding to the given code
     */
    static const PayloadType *typeFromCode(int code) {

        if (TYPES_TABLE.find(code) == TYPES_TABLE.end()) {
            std::cerr << "A payload type has not been associated with this code: ";
            std::cerr << code << std::endl;
            assert(false);
        } else {
            return &(TYPES_TABLE.at(code));
        }

    }
};


#endif //GS_MATTERHORN_PAYLOADTYPE_H
