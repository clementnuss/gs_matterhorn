#ifndef GS_MATTERHORN_DECODER_H
#define GS_MATTERHORN_DECODER_H

#include <map>
#include <cstdint>
#include <vector>
#include "DatagramSpec.h"

class Decoder {

public:
    void processByte(std::uint16_t);

    bool datagramReady();

    Datagram retrieveDatagram();

private:
    static const DecodingState INITIAL_STATE = DecodingState::SEEKING_FRAMESTART;

    void asyncRead();

    void processHeader(std::vector<uint16_t>);

    void processTelemetryPayload(std::vector<uint16_t>);

    void resetMachine();

    void seekHeader(uint16_t);

    void accumulateHeader(uint16_t);

    void seekControlFlag(uint16_t);

    void accumulatePayload(uint16_t);

    void validatePayload();

    void jumpToNextState();

    void assertBufferSmallerThan(size_t);

    void (Decoder::*action_)(uint16_t);

    std::vector<uint16_t> byteBuffer_;
    std::vector<uint16_t> checksumAccumulator_;
    DecodingState currentState_;
    Datagram currentDatagram_;


    const std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(uint16_t)>> STATES_TABLE{
            {DecodingState::SEEKING_FRAMESTART,   {DecodingState::PARSING_HEADER,       &Decoder::accumulateHeader}},
            {DecodingState::PARSING_HEADER,       {DecodingState::SEEKING_CONTROL_FLAG, &Decoder::seekControlFlag}},
            {DecodingState::SEEKING_CONTROL_FLAG, {DecodingState::PARSING_PAYLOAD,      &Decoder::accumulatePayload}},
            {DecodingState::PARSING_PAYLOAD,      {DecodingState::DATAGRAM_READY,       nullptr}}, //TODO: find more elegant solution
            {DecodingState::DATAGRAM_READY,       {DecodingState::SEEKING_FRAMESTART,   &Decoder::seekHeader}}
    };

};


#endif //GS_MATTERHORN_DECODER_H
