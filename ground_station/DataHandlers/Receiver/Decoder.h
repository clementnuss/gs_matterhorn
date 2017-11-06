#ifndef GS_MATTERHORN_DECODER_H
#define GS_MATTERHORN_DECODER_H

#include <map>
#include <cstdint>
#include <vector>
#include "DatagramSpec.h"
#include <chrono>
#include <FileLogger.h>

class Decoder {

public:

    Decoder();

    bool processByte(std::uint8_t);

    bool datagramReady();

    Datagram retrieveDatagram();

    DecodingState currentState() const;


public:
    static const DecodingState INITIAL_STATE = DecodingState::SEEKING_FRAMESTART;

//    void asyncRead();

    bool processHeader(std::vector<uint8_t>);

    void processTelemetryPayload(std::vector<uint8_t>);

    void resetMachine();

    void seekHeader(uint8_t);

    void accumulateHeader(uint8_t);

    void seekControlFlag(uint8_t);

    void accumulatePayload(uint8_t);

    void accumulateChecksum(uint8_t);

    bool validatePayload();

    void jumpToNextState();

    void assertBufferSmallerThan(size_t);

    static std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(uint8_t)>> createStatesMap() {
        return std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(uint8_t)>>{
                {DecodingState::SEEKING_FRAMESTART,   {DecodingState::PARSING_HEADER,       &Decoder::accumulateHeader}},
                {DecodingState::PARSING_HEADER,       {DecodingState::SEEKING_CONTROL_FLAG, &Decoder::seekControlFlag}},
                {DecodingState::SEEKING_CONTROL_FLAG, {DecodingState::PARSING_PAYLOAD,      &Decoder::accumulatePayload}},
                {DecodingState::PARSING_PAYLOAD,      {DecodingState::PARSING_CHECKSUM,     &Decoder::accumulateChecksum}},
                {DecodingState::PARSING_CHECKSUM,     {DecodingState::SEEKING_FRAMESTART,   &Decoder::seekHeader}}
        };
    }

    std::vector<uint8_t> byteBuffer_;
    std::vector<uint8_t> checksumAccumulator_;
    DecodingState currentState_;
    Datagram currentDatagram_;
    FileLogger logger_;
    std::chrono::system_clock::time_point startupTime_;

    static const std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(uint8_t)>> STATES_TABLE;

    void (Decoder::*action_)(uint8_t);

};


#endif //GS_MATTERHORN_DECODER_H
