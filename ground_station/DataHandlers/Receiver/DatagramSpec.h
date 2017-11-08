#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection" // disable "unused variable" code insight
#ifndef GS_MATTERHORN_PROTOCOL_H
#define GS_MATTERHORN_PROTOCOL_H

#include <map>
#include <vector>
#include <memory>
#include "DataStructures/datastructs.h"
#include "Factories.h"

typedef uint16_t checksum_t;

enum class DecodingState {
    SEEKING_FRAMESTART,
    PARSING_HEADER,
    SEEKING_CONTROL_FLAG,
    PARSING_PAYLOAD,
    PARSING_CHECKSUM,
    VALIDATING_PAYLOAD,
    DATAGRAM_READY
};

enum class DatagramPayloadType {
    TELEMETRY, Count
};

static const std::map<DatagramPayloadType, size_t> PAYLOAD_TYPES_LENGTH = {
        {DatagramPayloadType::TELEMETRY, 30}// TODO: couple DatagramPayloadType and size and factory function
};

struct Datagram {
    uint32_t sequenceNumber_;
    DatagramPayloadType payloadType_;
    std::shared_ptr<IDeserializable> deserializedPayload_;
    bool complete;
};

static constexpr uint8_t HEADER_PREAMBLE_FLAG = 0x55;
static constexpr uint8_t CONTROL_FLAG = 0xFF;

// Field sizes in bytes
static constexpr size_t SEQUENCE_NUMBER_SIZE = sizeof(uint32_t);
static constexpr size_t PAYLOAD_TYPE_SIZE = sizeof(uint8_t);

// Sections sizes in bytes
static constexpr size_t PREAMBLE_SIZE = 4;
static constexpr size_t HEADER_SIZE = SEQUENCE_NUMBER_SIZE + PAYLOAD_TYPE_SIZE;
static constexpr size_t CONTROL_FLAG_SIZE = 1;
static constexpr size_t CHECKSUM_SIZE = 2;


/**
 * This map specifies each payload type as well as its internal field lengths in bytes
 *
 * Any custom payload specification should be added here.
 */
static const std::map<DatagramPayloadType, std::shared_ptr<IDeserializable>(*)(
        std::vector<uint8_t>)> TELEMETRY_PAYLOAD_FACTORIES{
        {DatagramPayloadType::TELEMETRY, &Factories::telemetryReadingFactory}
};


#endif //GS_MATTERHORN_PROTOCOL_H

#pragma clang diagnostic pop