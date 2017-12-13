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


private:
    static const DecodingState INITIAL_STATE = DecodingState::SEEKING_FRAMESTART;

    bool processHeader(std::vector<uint8_t>);

    void processTelemetryPayload(std::vector<uint8_t>);

    void resetMachine();

    void seekHeader(uint8_t);

    void accumulateHeader(uint8_t);

    /**
     * Triggers a jump to the next state if the next byte received corresponds to the control flag,
     * resets the state of the decoder and log otherwise
     */
    void seekControlFlag(uint8_t);

    /**
     * Stores incoming bytes to the byte buffer until the number of bytes received corresponds
     * to the length of the payload as indicated by the payload type flag of the datagram
     */
    void accumulatePayload(uint8_t);

    /**
     * Stores incoming bytes to the byte buffer until the number of bytes received corresponds
     * to the length of the checksum
     */
    void accumulateChecksum(uint8_t);

    /**
     * Verifies that the payload corresponds to its checksum.
     *
     * @return True if the checksums match, false and log otherwise
     */
    bool validatePayload();

    /**
     * Triggers the jump to the next state
     */
    void jumpToNextState();

    void assertBufferSmallerThan(size_t);

    /**
     * State machine states table.
     *
     * Each state is associated with a function which knows how to handle the bytes received.
     * Once the function is satisfied with the data that has been received, it triggers the
     * jump to the next state
     *
     * @return A mapping from a state to the next state and the corresponfing decoding function
     */
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
