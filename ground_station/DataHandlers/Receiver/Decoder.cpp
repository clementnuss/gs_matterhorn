#include <cassert>
#include <iostream>
#include <utility>
#include <DataStructures/Datastructs.h>
#include <Utilities/TimeUtils.h>
#include "Decoder.h"
#include "Utilities/ParsingUtilities.h"


Decoder::Decoder(const string &logTitle) : byteBuffer_{},
                                           checksumAccumulator_{},
                                           currentState_{std::make_unique<SeekingFrameStart>()},
                                           currentDatagram_{},
                                           logger_{LogConstants::DECODER_LOG_PATH + logTitle},
                                           startupTime_{std::chrono::system_clock::now()} {}

bool
Decoder::processByte(uint8_t byte) {
    assert(!datagramReady());

    (*currentState_)(this, byte);

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

    uint8_t payloadInfo = headerBuffer[SEQUENCE_NUMBER_SIZE];

    uint8_t flyableType = (payloadInfo & CommunicationsConstants::FLYABLE_TYPE_MASK) >> 4;
    uint8_t payloadType = payloadInfo & CommunicationsConstants::PAYLOAD_TYPE_MASK;

    if (PayloadType::TYPES_TABLE.count(payloadType) > 0) {

        currentDatagram_.sequenceNumber_ = seqNum;
        currentDatagram_.flyableType_ = static_cast<FlyableType>(flyableType);
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
Decoder::resetMachine() {
    byteBuffer_.clear();
    checksumAccumulator_.clear();
    currentDatagram_ = Datagram();
    currentState_ = std::make_unique<SeekingFrameStart>();
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

bool
Decoder::validateATPayload() {

    assert(byteBuffer_.size() == ATCommandResponse::CHECKSUM_SIZE);

    uint8_t checksum{0xFF};

    for (const auto &byte : checksumAccumulator_) {
        checksum -= byte;
    }

    if (checksum == byteBuffer_.front()) {

        currentDatagram_.complete = true;
        return true;

    } else {

        // Log error
        stringstream ss;
        ss << setw(PrintConstants::FIELD_WIDTH) << msecsBetween(startupTime_, std::chrono::system_clock::now());
        ss << PrintConstants::DELIMITER;
        ss << "Invalid AT checksum, expected: ";
        ss << setw(PrintConstants::FIELD_WIDTH) << std::hex << byteBuffer_[0];
        ss << " got: ";
        ss << std::hex << checksum;
        logger_.registerString(ss.str());

        return false;

    }
}

void
Decoder::assertBufferSmallerThan(size_t bound) {
    assert(0 <= byteBuffer_.size() && byteBuffer_.size() < bound);
}

IState *
Decoder::currentState() const {
    return currentState_.get();
}

void
SeekingFrameStart::operator()(Decoder *context, const uint8_t &byte) const {

    if (byte == ATCommandResponse::FRAME_DELIMITER) {
        context->byteBuffer_.clear();
        context->currentDatagram_.payloadType_ = &PayloadType::AT_COMMAND;
        context->currentState_ = std::make_unique<ParsingATCommandHeader>();
    } else {
        context->assertBufferSmallerThan(PREAMBLE_SIZE);
        if (byte == HEADER_PREAMBLE_FLAG) {
            context->byteBuffer_.push_back(byte);
        } else {
            context->byteBuffer_.clear();
        }

        if (context->byteBuffer_.size() == PREAMBLE_SIZE) {
            context->byteBuffer_.clear();
            context->currentState_ = std::make_unique<ParsingHeader>();
        }
    }
}

void
ParsingHeader::operator()(Decoder *context, const uint8_t &byte) const {
    context->assertBufferSmallerThan(HEADER_SIZE);

    context->byteBuffer_.push_back(byte);

    if (context->byteBuffer_.size() == HEADER_SIZE) {
        for (auto b : context->byteBuffer_) {
            context->checksumAccumulator_.push_back(b);
        }

        if (context->processHeader(context->byteBuffer_)) {
            context->byteBuffer_.clear();
            context->currentState_ = std::make_unique<SeekingControlFlag>();
        } else {
            context->resetMachine();
        }
    }
}

void
SeekingControlFlag::operator()(Decoder *context, const uint8_t &byte) const {
    assert(context->byteBuffer_.empty());

    if (byte == CONTROL_FLAG) {
        context->currentState_ = std::make_unique<ParsingPayload>();
    } else {
        //TODO: log
        context->resetMachine();
    }
}

void
ParsingPayload::operator()(Decoder *context, const uint8_t &byte) const {
    context->assertBufferSmallerThan(context->currentDatagram_.payloadType_->length());

    context->byteBuffer_.push_back(byte);
    context->checksumAccumulator_.push_back(byte);

    if (context->byteBuffer_.size() == context->currentDatagram_.payloadType_->length()) {
        context->currentDatagram_.payloadData_ = context->byteBuffer_;
        context->byteBuffer_.clear();
        context->currentState_ = std::make_unique<ParsingChecksum>();
    }
}

void
ParsingChecksum::operator()(Decoder *context, const uint8_t &byte) const {
    context->assertBufferSmallerThan(CHECKSUM_SIZE);

    context->byteBuffer_.push_back(byte);

    if (context->byteBuffer_.size() == CHECKSUM_SIZE) {
        if (context->validatePayload()) {
            context->byteBuffer_.clear();
            context->checksumAccumulator_.clear();
            context->currentState_ = std::make_unique<SeekingFrameStart>();
        } else {
            context->resetMachine();
        }
    }
}

void
ParsingATCommandHeader::operator()(Decoder *context, const uint8_t &byte) const {
    context->assertBufferSmallerThan(ATCommandResponse::HEADER_SIZE);

    context->byteBuffer_.push_back(byte);

    if (context->byteBuffer_.size() == ATCommandResponse::HEADER_SIZE) {
        context->byteBuffer_.clear();
        context->currentState_ = std::make_unique<ParsingATCommandPayload>();
    }
}

void
ParsingATCommandPayload::operator()(Decoder *context, const uint8_t &byte) const {
    context->assertBufferSmallerThan(ATCommandResponse::PAYLOAD_SIZE);

    context->byteBuffer_.push_back(byte);
    context->checksumAccumulator_.push_back(byte);

    if (context->byteBuffer_[0] != ATCommandResponse::FRAME_TYPE) {
        // This is not an AT response, bail out
        context->resetMachine();
    }

    if (context->byteBuffer_.size() == ATCommandResponse::PAYLOAD_SIZE) {

        context->currentDatagram_.payloadData_ = context->byteBuffer_;
        context->byteBuffer_.clear();
        context->currentState_ = std::make_unique<ParsingATCommandChecksum>();

    }
}

void
ParsingATCommandChecksum::operator()(Decoder *context, const uint8_t &byte) const {

    context->assertBufferSmallerThan(ATCommandResponse::CHECKSUM_SIZE);

    context->byteBuffer_.push_back(byte);

    if (context->byteBuffer_.size() == ATCommandResponse::CHECKSUM_SIZE) {
        if (context->validateATPayload()) {
            context->byteBuffer_.clear();
            context->checksumAccumulator_.clear();
            context->currentState_ = std::make_unique<SeekingFrameStart>();
        } else {
            context->resetMachine();
        }
    }
}
