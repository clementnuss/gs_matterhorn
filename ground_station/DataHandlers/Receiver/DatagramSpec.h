#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection" // disable "unused variable" code insight
#ifndef GS_MATTERHORN_PROTOCOL_H
#define GS_MATTERHORN_PROTOCOL_H

#include <map>
#include <vector>

enum class DecodingState {
    SEEKING_FRAMESTART, PARSING_HEADER, SEEKING_CONTROL_FLAG, PARSING_PAYLOAD, DATAGRAM_READY
};

enum class DatagramPayloadType {
    TELEMETRY = 0x00, ROCKET_PAYLOAD = 0x01
};

static const std::map<DatagramPayloadType, size_t> PAYLOAD_TYPES_LENGTH = {
        {DatagramPayloadType::TELEMETRY,      34},
        {DatagramPayloadType::ROCKET_PAYLOAD, -1}   // TODO: handle ROCKET_PAYLOAD variable sizeCONTROL_FLAG
};

struct Datagram {
    uint32_t sequenceNumber;
    DatagramPayloadType payloadType;
//    std::vector<uint32_t> payload;
    TelemetryReading payload;
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
#endif //GS_MATTERHORN_PROTOCOL_H

#pragma clang diagnostic pop