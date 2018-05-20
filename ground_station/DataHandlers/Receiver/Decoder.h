#ifndef GS_MATTERHORN_DECODER_H
#define GS_MATTERHORN_DECODER_H

#include <map>
#include <cstdint>
#include <vector>
#include "DatagramSpec.h"
#include <chrono>
#include <Loggers/FileLogger.h>

class Decoder;

class IState {
public:
    virtual void operator()(Decoder *, const uint8_t &) const = 0;
};

/**
 * Detects a consecutive sequence of preamble bytes of length specified by the datagram specifications
 * and trigger a jump to the next state once the sequence has been found
 */
class SeekingFrameStart : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

/**
 * Stores incoming bytes to the byte buffer until the number of bytes received corresponds
 * to the length of the header
 */
class ParsingHeader : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

/**
 * Triggers a jump to the next state if the next byte received corresponds to the control flag,
 * resets the state of the decoder and log otherwise
 */
class SeekingControlFlag : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

/**
 * Stores incoming bytes to the byte buffer until the number of bytes received corresponds
 * to the length of the payload as indicated by the payload type flag of the datagram
 */
class ParsingPayload : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

/**
* Stores incoming bytes to the byte buffer until the number of bytes received corresponds
* to the length of the checksum
*/
class ParsingChecksum : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

class ParsingATCommandHeader : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

class ParsingATCommandPayload : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};

class ParsingATCommandChecksum : public IState {
    void operator()(Decoder *, const uint8_t &) const override;
};


/**
 * The Decoder class is in charge of interpreting bytes received trough the radio module.
 * It gathers bytes and treats them as specified by the datagram
 */
class Decoder {

    friend class SeekingFrameStart;

    friend class ParsingHeader;

    friend class SeekingControlFlag;

    friend class ParsingPayload;

    friend class ParsingChecksum;

    friend class parsingATCommandHeader;

    friend class parsingATCommandPayload;

    friend class ParsingATCommandChecksum;

public:

    Decoder(const string &logTitle);

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
    IState *currentState() const;


private:

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
     * Verifies that the payload corresponds to its checksum.
     *
     * @return True if the checksums match, false and log otherwise
     */
    bool validatePayload();

    void assertBufferSmallerThan(size_t);

    std::vector<uint8_t> byteBuffer_;
    std::vector<uint8_t> checksumAccumulator_;
    std::unique_ptr<IState> currentState_;
    Datagram currentDatagram_;
    FileLogger logger_;
    std::chrono::system_clock::time_point startupTime_;
};


#endif //GS_MATTERHORN_DECODER_H
