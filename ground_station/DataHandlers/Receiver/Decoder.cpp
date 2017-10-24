#include <cassert>
#include <iostream>
#include <DataStructures/datastructs.h>
#include "Decoder.h"

Decoder::Decoder() {
    currentState_ = INITIAL_STATE;
    action_ = &Decoder::seekHeader;
}

bool Decoder::processByte(uint8_t byte) {
    assert(currentState_ != DecodingState::DATAGRAM_READY);

    (this->*action_)(byte);

    return this->datagramReady();
}

void Decoder::processHeader(std::vector<uint8_t> headerBuffer) {
    assert(headerBuffer.size() == HEADER_SIZE);

    uint32_t seqNum = 0;
    for (size_t i = 0; i < SEQUENCE_NUMBER_SIZE; i++) {
        seqNum <<= 8 * sizeof(uint8_t);
        seqNum |= headerBuffer[i];
    }

    uint8_t payloadType = headerBuffer[SEQUENCE_NUMBER_SIZE];

    currentDatagram_.sequenceNumber = seqNum;
    currentDatagram_.payloadType = DatagramPayloadType(payloadType);
}

//TODO: find elegant way to abstract the way the payload is treated from the decoder implementation
//TODO: maybe in a map in DatagramSpec.h
void Decoder::processTelemetryPayload(std::vector<uint8_t> payloadBuffer) {
    switch (currentDatagram_.payloadType) {
        case DatagramPayloadType::TELEMETRY: {

            auto it = payloadBuffer.begin();

            long measurement_time = static_cast<long>(parseUint32(it));

            uint16_t ax = parseUint16(it);
            uint16_t ay = parseUint16(it);
            uint16_t az = parseUint16(it);

            uint16_t mx = parseUint16(it);
            uint16_t my = parseUint16(it);
            uint16_t mz = parseUint16(it);

            uint16_t gx = parseUint16(it);
            uint16_t gy = parseUint16(it);
            uint16_t gz = parseUint16(it);

            float_cast temperature = {.uint32 = parseUint32(it)};
            float_cast pressure = {.uint32 = parseUint32(it)};
            float_cast altitude = {.uint32 = parseUint32(it)};

            currentDatagram_.payload = TelemetryReading{
                    measurement_time, {altitude.fl, true},
                    {ax, ay, az, true}, {mx, my, mz, true}, {gx, gy, gz, true},
                    {pressure.fl, true}, {temperature.fl, true}};

        }
            break;
        case DatagramPayloadType::ROCKET_PAYLOAD:
            //TODO: implement rocket_event
            break;
    }
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
        if (validatePayload()) {
            processTelemetryPayload(byteBuffer_);
            jumpToNextState();
        } else {
            resetMachine();
        }
    }
}

bool Decoder::validatePayload() {

    //TODO: compute CRC checksum with checksumAccumulator.
    return checksumAccumulator_.size() > 1;
}

void Decoder::assertBufferSmallerThan(size_t bound) {
    assert(0 <= byteBuffer_.size() && byteBuffer_.size() < bound);
}

DecodingState Decoder::getCurrentState_() const {
    return currentState_;
}

uint16_t parseUint16(vector<uint8_t>::iterator &it) {
    return (*(it++) << 8) | *(it++);
}

uint32_t parseUint32(vector<uint8_t>::iterator &it) {
    return (*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}