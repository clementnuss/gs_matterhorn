#ifndef GS_MATTERHORN_PROTOCOL_H
#define GS_MATTERHORN_PROTOCOL_H

#include <map>
#include <vector>

enum class DecodingState {
    SEEKING_FRAMESTART, PARSING_HEADER, SEEKING_CONTROL_FLAG, PARSING_PAYLOAD, DATAGRAM_READY
};

enum class DatagramPayloadType {
    TELEMETRY, ROCKET_PAYLOAD
};

static const std::map<DatagramPayloadType, size_t> PAYLOAD_TYPES_LENGTH = {
        {DatagramPayloadType::TELEMETRY,      28},
        {DatagramPayloadType::ROCKET_PAYLOAD, -1}
};

struct Datagram {
    uint32_t sequenceNumber;
    DatagramPayloadType payloadType;
    std::vector<uint16_t> payload;
};

static constexpr uint16_t HEADER_PREAMBLE_FLAG = 0x55;
static constexpr uint16_t CONTROL_FLAG = 0xFF;

// Field sizes in bytes
static constexpr size_t SEQUENCE_NUMBER_SIZE = sizeof(uint32_t);
static constexpr size_t PAYLOAD_TYPE_SIZE = sizeof(uint8_t);

// Sections sizes in bytes
static constexpr size_t PREAMBLE_SIZE = 4;
static constexpr size_t HEADER_SIZE = SEQUENCE_NUMBER_SIZE + PAYLOAD_TYPE_SIZE;
static constexpr size_t CONTROL_FLAG_SIZE = 1;
static constexpr size_t CHECKSUM_SIZE = 2;
#endif //GS_MATTERHORN_PROTOCOL_H
