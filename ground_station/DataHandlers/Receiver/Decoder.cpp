#include <cassert>
#include "Decoder.h"

void Decoder::processByte(uint8_t byte) {
    assert(currentState_ != DecodingState::DATAGRAM_READY);

    (this->*action_)(byte);
}

void Decoder::processHeader(std::vector<uint8_t> headerBuffer) {
    assert(headerBuffer.size() == HEADER_SIZE);

    uint32_t seqNum = 0;
    for (size_t i = 0; i < SEQUENCE_NUMBER_SIZE; i++) {
        seqNum = seqNum & headerBuffer[i];
        seqNum = seqNum << sizeof(uint8_t);
    }

    uint8_t payloadType = headerBuffer[SEQUENCE_NUMBER_SIZE];

    currentDatagram_.sequenceNumber = seqNum;
    currentDatagram_.payloadType = DatagramPayloadType(payloadType);
}

//TODO: find elegant way to abstract the way the payload is treated from the decoder implementation
//TODO: maybe in a map in DatagramSpec.h
void Decoder::processTelemetryPayload(std::vector<uint8_t> payloadBuffer) {

}

bool Decoder::datagramReady() {
    return currentState_ == DecodingState::DATAGRAM_READY;
}

Datagram Decoder::retrieveDatagram() {
    assert(datagramReady());

    Datagram r = currentDatagram_;
    resetMachine();

    return r;
}

void Decoder::jumpToNextState() {
    auto pair = STATES_TABLE.at(currentState_);

    currentState_ = pair.first;
    action_ = pair.second;
}

void Decoder::resetMachine() {
    byteBuffer_.clear();
    checksumAccumulator_.clear();
    currentDatagram_ = Datagram();
    currentState_ = INITIAL_STATE;
}

void Decoder::seekHeader(uint8_t byte) {
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

void Decoder::accumulateHeader(uint8_t byte) {
    assertBufferSmallerThan(HEADER_SIZE);

    byteBuffer_.push_back(byte);

    if (byteBuffer_.size() == HEADER_SIZE) {
        for (auto b : byteBuffer_) {
            checksumAccumulator_.push_back(b);
        }
        processHeader(byteBuffer_);
        byteBuffer_.clear();
        jumpToNextState();
    }
}

void Decoder::seekControlFlag(uint8_t byte) {
    assert(byteBuffer_.empty());

    if (byte == CONTROL_FLAG) {
        checksumAccumulator_.push_back(byte);
        jumpToNextState();
    } else {
        resetMachine();
    }
}

void Decoder::accumulatePayload(uint8_t byte) {
    assertBufferSmallerThan(PAYLOAD_TYPES_LENGTH.at(currentDatagram_.payloadType));

    byteBuffer_.push_back(byte);
    checksumAccumulator_.push_back(byte);

    if (byteBuffer_.size() == PAYLOAD_TYPES_LENGTH.at(currentDatagram_.payloadType)) {
        processTelemetryPayload(byteBuffer_);
        byteBuffer_.clear();
        validatePayload();
    }
}

void Decoder::validatePayload() {

    //TODO: compute CRC checksum with checksumAccumulator.
    if (true) {
        jumpToNextState();
    }
}

void Decoder::assertBufferSmallerThan(size_t bound) {
    assert(0 <= byteBuffer_.size() && byteBuffer_.size() < bound);
}