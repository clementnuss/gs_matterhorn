
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

private:
    int code_;
    size_t length_;

    std::shared_ptr<IDeserializable> (*factoryFunc_)(std::vector<uint8_t>);

    PayloadType(int code, size_t length, std::shared_ptr<IDeserializable>(*factoryFunc)(std::vector<uint8_t>)) :
            code_{code}, length_{length}, factoryFunc_{factoryFunc} {}

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
     * @return The code associated with this payload type.
     */
    int code() const {
        return code_;
    }

    /**
     *
     * @return The length associated with this payload type.
     */
    size_t length() const {
        return length_;
    }

    //TODO: change operator overloading to simple function name such as 'process'
    std::shared_ptr<IDeserializable> operator()(std::vector<uint8_t> bytes) const {
        return factoryFunc_(bytes);
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
        switch (code) {
            case 0:
                return &TELEMETRY;
            default:
                std::cerr << "A payload type has not been associated with this code: ";
                std::cerr << code << std::endl;
                assert(false);
                break;

        }
    }
};


#endif //GS_MATTERHORN_PAYLOADTYPE_H
