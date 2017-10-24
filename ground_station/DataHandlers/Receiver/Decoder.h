#ifndef GS_MATTERHORN_DECODER_H
#define GS_MATTERHORN_DECODER_H

#include <map>
#include <cstdint>
#include <vector>
#include "DatagramSpec.h"

typedef union {
    float fl;
    uint32_t uint32;
} float_cast;

uint16_t parseUint16(vector<uint8_t>::iterator &it);

uint32_t parseUint32(vector<uint8_t>::iterator &it);

class Decoder {

public:

    Decoder();

    bool processByte(std::uint8_t);

    bool datagramReady();

    Datagram retrieveDatagram();

    DecodingState getCurrentState_() const;


private:
    static const DecodingState INITIAL_STATE = DecodingState::SEEKING_FRAMESTART;

//    void asyncRead();

    void processHeader(std::vector<uint8_t>);

    void processTelemetryPayload(std::vector<uint8_t>);

    void processGenericTelemetryPayload(std::vector<uint8_t>);

    void resetMachine();

    void seekHeader(uint8_t);

    void accumulateHeader(uint8_t);

    void seekControlFlag(uint8_t);

    void accumulatePayload(uint8_t);

    bool validatePayload();

    void jumpToNextState();

    void assertBufferSmallerThan(size_t);

    void (Decoder::*action_)(uint8_t);

    std::vector<uint8_t> byteBuffer_;
    std::vector<uint8_t> checksumAccumulator_;
    DecodingState currentState_;
    Datagram currentDatagram_;


    const std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(uint8_t)>> STATES_TABLE{
            {DecodingState::SEEKING_FRAMESTART,   {DecodingState::PARSING_HEADER,       &Decoder::accumulateHeader}},
            {DecodingState::PARSING_HEADER,       {DecodingState::SEEKING_CONTROL_FLAG, &Decoder::seekControlFlag}},
            {DecodingState::SEEKING_CONTROL_FLAG, {DecodingState::PARSING_PAYLOAD,      &Decoder::accumulatePayload}},
            {DecodingState::PARSING_PAYLOAD,      {DecodingState::DATAGRAM_READY,       nullptr}}, //TODO: find more elegant solution
            {DecodingState::DATAGRAM_READY,       {DecodingState::SEEKING_FRAMESTART,   &Decoder::seekHeader}}
    };

};


#endif //GS_MATTERHORN_DECODER_H
