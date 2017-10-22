#include <cassert>
#include "Decoder.h"

void Decoder::processByte(uint16_t byte) {
    assert(currentState_ != DecodingState::DATAGRAM_READY);

    (this->*action_)(byte);
}

void Decoder::processHeader(std::vector<uint16_t> headerBuffer) {

}

//TODO: find elegant way to abstract the way the payload is treated from the decoder implementation
//TODO: maybe in a map in DatagramSpec.h
void Decoder::processTelemetryPayload(std::vector<uint16_t> payloadBuffer) {

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

void Decoder::seekHeader(uint16_t byte) {
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

void Decoder::accumulateHeader(uint16_t byte) {
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

void Decoder::seekControlFlag(uint16_t byte) {
    assert(byteBuffer_.empty());

    if (byte == CONTROL_FLAG) {
        checksumAccumulator_.push_back(byte);
        jumpToNextState();
    } else {
        resetMachine();
    }
}

void Decoder::accumulatePayload(uint16_t byte) {
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