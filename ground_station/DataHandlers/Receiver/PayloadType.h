
#ifndef GS_MATTERHORN_PAYLOADTYPE_H
#define GS_MATTERHORN_PAYLOADTYPE_H


#include <DataStructures/datastructs.h>
#include <memory>
#include <vector>
#include <map>
#include <cassert>
#include <iostream>

class PayloadType {
public:
    static const PayloadType TELEMETRY;

private:
    int code_;
    size_t length_;

    std::shared_ptr<IDeserializable> (*factoryFunc_)(std::vector<uint8_t>);

    PayloadType(int code, size_t length, std::shared_ptr<IDeserializable>(*factoryFunc)(std::vector<uint8_t>)) :
            code_{code}, length_{length}, factoryFunc_{factoryFunc} {}

public:
    PayloadType &operator=(const PayloadType &) = default;

    operator int() const {
        return code_;
    }

    int code() const {
        return code_;
    }

    size_t length() const {
        return length_;
    }

    std::shared_ptr<IDeserializable> operator()(std::vector<uint8_t> bytes) const {
        return factoryFunc_(bytes);
    }

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
