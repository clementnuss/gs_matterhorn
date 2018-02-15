#ifndef GS_MATTERHORN_DECODER_H
#define GS_MATTERHORN_DECODER_H

#include <map>
#include <cstdint>
#include <vector>
#include "DatagramSpec.h"
#include <chrono>
#include <Loggers/FileLogger.h>

/**
 * The Decoder class is in charge of interpreting bytes received trough the radio module.
 * It gathers bytes and treats them as specified by the datagram
 */
class Decoder {

public:

    Decoder();

    /**
     * Register an incoming byte as input to the decoder
     *
     * @return True if the input resulted in an overall interpretable datagram whose contents are ready to be
     * retrieved from the decoder
     */
    bool processByte(std::uint8_t);

    /**
     *
     * @return True if the decoder successfully decoded a datagram which is available for retrieval
     */
    bool datagramReady();

    /**
     *
     * @return The decoded datagram and resets the machine
     */
    Datagram retrieveDatagram();

    /**
     *
     * @return The current state the machine is in. Mainly used for testing purposes
     */
    DecodingState currentState() const;


private:
    static const DecodingState INITIAL_STATE = DecodingState::SEEKING_FRAMESTART;

    /**
     * Extract from the header bytes information about the datagram such as the type of its payload and the
     * sequence number
     *
     * @return True if the extraction was successful, false and log otherwise
     */
    bool processHeader(std::vector<uint8_t>);

    /**
     * Empties all the buffers and resets the state of the machine to its initial state
     */
    void resetMachine();

    /**
     * Detects a consecutive sequence of preamble bytes of length specified by the datagram specifications
     * and trigger a jump to the next state once the sequence has been found
     */
    void seekHeader(uint8_t);

    /**
     * Stores incoming bytes to the byte buffer until the number of bytes received corresponds
     * to the length of the header
     */
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
