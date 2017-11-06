
#ifndef GS_MATTERHORN_PAYLOADTYPE_H
#define GS_MATTERHORN_PAYLOADTYPE_H


#include <DataStructures/datastructs.h>
#include <memory>
#include <vector>
#include <map>

class PayloadType {
public:
    static const PayloadType TELEMETRY;
    static const PayloadType NONE;

private:
    int code_;
    size_t length_;

    std::shared_ptr<IDeserializable> (*factoryFunc_)(std::vector<uint8_t>);

    //TODO: make this static
    const std::map<int, PayloadType> typeForCode_{
            {0, TELEMETRY}
    };

private:
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

    const PayloadType *operator()(int i) const {
        return &typeForCode_.at(i);
    }
};


#endif //GS_MATTERHORN_PAYLOADTYPE_H
