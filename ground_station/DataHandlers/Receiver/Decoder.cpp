#include <cassert>
#include <iostream>
#include <utility>
#include <DataStructures/datastructs.h>
#include <Utilities/TimeUtils.h>
#include "Decoder.h"
#include "Utilities/ParsingUtilities.h"

const std::map<DecodingState, std::pair<DecodingState, void (Decoder::*)(
        uint8_t)>> Decoder::STATES_TABLE = Decoder::createStatesMap();

Decoder::Decoder() : byteBuffer_{}, checksumAccumulator_{}, currentState_{INITIAL_STATE}, currentDatagram_{},
                     logger_{LogConstants::DECODER_LOG_PATH}, startupTime_{std::chrono::system_clock::now()},
                     action_{&Decoder::seekHeader} {}

bool
Decoder::processByte(uint8_t byte) {
    assert(!datagramReady());

    (this->*action_)(byte);

    return this->datagramReady();
}

bool
Decoder::processHeader(std::vector<uint8_t> headerBuffer) {
    assert(headerBuffer.size() == HEADER_SIZE);

    uint32_t seqNum = 0;
    for (size_t i = 0; i < SEQUENCE_NUMBER_SIZE; i++) {


        seqNum <<= 8 * sizeof(uint8_t);
        seqNum |= headerBuffer[i];

    }

    uint8_t payloadType = headerBuffer[SEQUENCE_NUMBER_SIZE];

    if (PayloadType::TYPES_TABLE.count(payloadType) > 0) {

        currentDatagram_.sequenceNumber_ = seqNum;
        currentDatagram_.payloadType_ = PayloadType::typeFromCode(payloadType);
        return true;

    } else {

        // Log error
        stringstream ss;
        ss << setw(PrintConstants::FIELD_WIDTH) << msecsBetween(startupTime_, std::chrono::system_clock::now());
        ss << PrintConstants::DELIMITER;
        ss << "Wrong datagram payload type: ";
        ss << "0x" << setw(2) << setfill('0') << std::hex << static_cast<int>(payloadType);
        logger_.registerString(ss.str());

        return false;
    };
}

bool
Decoder::datagramReady() {
    return currentDatagram_.complete;
}

Datagram
Decoder::retrieveDatagram() {
    assert(datagramReady());

    Datagram r = currentDatagram_;
    resetMachine();

    return r;
}

void
Decoder::jumpToNextState() {
    auto pair = STATES_TABLE.at(currentState_);

    currentState_ = pair.first;
    action_ = pair.second;
}

void
Decoder::resetMachine() {
    byteBuffer_.clear();
    checksumAccumulator_.clear();
    currentDatagram_ = Datagram();
    currentState_ = INITIAL_STATE;
    action_ = &Decoder::seekHeader;
}

void
Decoder::seekHeader(uint8_t byte) {
    assertBufferSmallerThan(PREAMBLE_SIZE);

    if (byte == HEADER_PREAMBLE_FLAG) {
        byteBuffer_.push_back(byte);
    } else {
        byteBuffer_.clear();
    }

    if (byteBuffer_.size() == PREAMBLE_SIZE) {
        byteBuffer_.clear();
        jumpToNextState();
    }
}

void
Decoder::accumulateHeader(uint8_t byte) {
    assertBufferSmallerThan(HEADER_SIZE);

    byteBuffer_.push_back(byte);

    if (byteBuffer_.size() == HEADER_SIZE) {
        for (auto b : byteBuffer_) {
            checksumAccumulator_.push_back(b);
        }

        if (processHeader(byteBuffer_)) {
            byteBuffer_.clear();
            jumpToNextState();
        } else {
            resetMachine();
        }
    }
}

void
Decoder::seekControlFlag(uint8_t byte) {
    assert(byteBuffer_.empty());

    if (byte == CONTROL_FLAG) {
        jumpToNextState();
    } else {
        //TODO: log
        resetMachine();
    }
}

void
Decoder::accumulatePayload(uint8_t byte) {
    assertBufferSmallerThan(currentDatagram_.payloadType_->length());

    byteBuffer_.push_back(byte);
    checksumAccumulator_.push_back(byte);

    if (byteBuffer_.size() == currentDatagram_.payloadType_->length()) {
        currentDatagram_.payloadData_ = byteBuffer_;
        byteBuffer_.clear();
        jumpToNextState();
    }
}

void
Decoder::accumulateChecksum(uint8_t byte) {
    assertBufferSmallerThan(CHECKSUM_SIZE);

    byteBuffer_.push_back(byte);

    if (byteBuffer_.size() == CHECKSUM_SIZE) {
        if (validatePayload()) {
            jumpToNextState();
        } else {
            resetMachine();
        }
    }
}

bool
Decoder::validatePayload() {

    assert(byteBuffer_.size() == CHECKSUM_SIZE);

    uint16_t crc = CRC::Calculate(&checksumAccumulator_[0], checksumAccumulator_.size(),
                                  CommunicationsConstants::CRC_16_GENERATOR_POLY);

    std::vector<uint8_t>::const_iterator it = byteBuffer_.begin();
    auto receivedCRC = parse16<checksum_t>(it);

    if (crc == receivedCRC) {

        currentDatagram_.complete = true;
        return true;

    } else {

        // Log error
        stringstream ss;
        ss << setw(PrintConstants::FIELD_WIDTH) << msecsBetween(startupTime_, std::chrono::system_clock::now());
        ss << PrintConstants::DELIMITER;
        ss << "Invalid checksum, expected: ";
        ss << setw(PrintConstants::FIELD_WIDTH) << std::hex << receivedCRC;
        ss << " got: ";
        ss << std::hex << crc;
        logger_.registerString(ss.str());

        return false;

    }
}

void
Decoder::assertBufferSmallerThan(size_t bound) {
    assert(0 <= byteBuffer_.size() && byteBuffer_.size() < bound);
}

DecodingState
Decoder::currentState() const {
    return currentState_;
}
