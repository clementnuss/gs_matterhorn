#include <DataHandlers/TelemetryHandler.h>
#include <gtest/gtest.h>
#include <DataHandlers/Receiver/Decoder.h>
#include <Utilities/RandUtils.h>
#include <Utilities/SensorUtils.h>

static constexpr double epsilon = 1e-3;

static void push3D(std::vector<uint8_t> &v, size_t byteCount, uint32_t a, uint32_t b, uint32_t c) {
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(a >> (8 * i)));
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(b >> (8 * i)));
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(c >> (8 * i)));
}

//TODO: remove code duplicate among datagram creation functions
static vector<uint8_t>
createDatagram(uint32_t seqnum, uint32_t timestamp, int16_t ax, int16_t ay, int16_t az, int16_t mx, int16_t my,
               int16_t mz, int16_t gx, int16_t gy, int16_t gz, float temp, float pres, int16_t pitot) {
    // Create datagram header
    vector<uint8_t> datagram{
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG};
    vector<uint8_t> checksumAccumulator{};

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
    }

    datagram.push_back(static_cast<uint8_t>(PayloadType::TELEMETRY));
    checksumAccumulator.push_back(static_cast<uint8_t>(PayloadType::TELEMETRY));
    datagram.push_back(CONTROL_FLAG);

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
    }

    push3D(datagram, 2, ax, ay, az);
    push3D(checksumAccumulator, 2, ax, ay, az);
    push3D(datagram, 2, gx, gy, gz);
    push3D(checksumAccumulator, 2, gx, gy, gz);
    push3D(datagram, 2, mx, my, mz);
    push3D(checksumAccumulator, 2, mx, my, mz);

    float_cast temperature{.fl = temp};
    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(temperature.uint32 >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(temperature.uint32 >> (8 * i)));
    }


    float_cast pressure{.fl = pres};
    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(pressure.uint32 >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(pressure.uint32 >> (8 * i)));
    }

    for (int i = 1; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(pitot >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(pitot >> (8 * i)));
    }

    checksum_t crc = CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                    CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = sizeof(checksum_t) - 1; i >= 0; i--) {
        datagram.push_back(static_cast<uint8_t>(crc >> (8 * i)));
    }

    return datagram;
}


static vector<uint8_t>
createEventDatagram(uint32_t seqnum, uint32_t timestamp, uint8_t code) {
    // Create datagram header
    vector<uint8_t> datagram{
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG};
    vector<uint8_t> checksumAccumulator{};

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
    }

    datagram.push_back(static_cast<uint8_t>(PayloadType::EVENT));
    checksumAccumulator.push_back(static_cast<uint8_t>(PayloadType::EVENT));
    datagram.push_back(CONTROL_FLAG);

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
    }

    datagram.emplace_back(code);
    checksumAccumulator.emplace_back(code);

    checksum_t crc = CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                    CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = sizeof(checksum_t) - 1; i >= 0; i--) {
        datagram.push_back(static_cast<uint8_t>(crc >> (8 * i)));
    }

    return datagram;
}

static vector<uint8_t>
createControlDatagram(uint32_t seqnum, uint32_t timestamp, int8_t partCode, uint16_t statusValue) {
    // Create datagram header
    vector<uint8_t> datagram{
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG};
    vector<uint8_t> checksumAccumulator{};

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));
    }

    datagram.push_back(static_cast<uint8_t>(PayloadType::CONTROL));
    checksumAccumulator.push_back(static_cast<uint8_t>(PayloadType::CONTROL));
    datagram.push_back(CONTROL_FLAG);

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
    }

    datagram.emplace_back(partCode);
    checksumAccumulator.emplace_back(partCode);

    for (int i = 1; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(statusValue >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(statusValue >> (8 * i)));
    }

    checksum_t crc = CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                    CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = sizeof(checksum_t) - 1; i >= 0; i--) {
        datagram.push_back(static_cast<uint8_t>(crc >> (8 * i)));
    }

    return datagram;
}

static void feedWithValidPreamble(Decoder &decoder) {
    if (decoder.currentState() != DecodingState::SEEKING_FRAMESTART) {
        cerr << " problem " << endl;
    }

    decoder.processByte(HEADER_PREAMBLE_FLAG);
    decoder.processByte(HEADER_PREAMBLE_FLAG);
    decoder.processByte(HEADER_PREAMBLE_FLAG);
    decoder.processByte(HEADER_PREAMBLE_FLAG);

    if (decoder.currentState() != DecodingState::PARSING_HEADER) {
        cerr << " problem " << endl;
    }
}

static void feedWithValidSequenceNumber(Decoder &decoder) {
    ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_HEADER);

    decoder.processByte(0x01);
    decoder.processByte(0x01);
    decoder.processByte(0x01);
    decoder.processByte(0x01);

    ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_HEADER);
}

static void feedWithValidPayloadType(Decoder &decoder) {
    // Select randomly one of the datagram payload types
    decoder.processByte(
            static_cast<uint8_t>(rand() % PayloadType::TYPES_TABLE.size()));
}

static void feedWithValidHeader(Decoder &decoder) {

    feedWithValidPreamble(decoder);
    feedWithValidSequenceNumber(decoder);
    feedWithValidPayloadType(decoder);

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_CONTROL_FLAG);

    decoder.processByte(CONTROL_FLAG);

    ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_PAYLOAD);
}


static void parsePacket(Decoder &decoder, vector<uint8_t> &datagram, PayloadType payloadType, Datagram *out) {

    size_t k = 0;

    // Process preamble
    for (size_t i = 0; i < PREAMBLE_SIZE; i++) {
        ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);
        ASSERT_FALSE(decoder.datagramReady());
        decoder.processByte(datagram[k++]);
    }

    // Process header
    for (size_t i = 0; i < HEADER_SIZE; i++) {
        ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_HEADER);
        ASSERT_FALSE(decoder.datagramReady());
        decoder.processByte(datagram[k++]);
    }

    // Process control flag
    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_CONTROL_FLAG);
    ASSERT_FALSE(decoder.datagramReady());
    decoder.processByte(datagram[k++]);

    // Process payload
    for (size_t i = 0; i < payloadType.length(); i++) {
        ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_PAYLOAD);
        ASSERT_FALSE(decoder.datagramReady());
        decoder.processByte(datagram[k++]);
    }

    // Process checksum
    for (size_t i = 0; i < CHECKSUM_SIZE; i++) {
        ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_CHECKSUM);
        ASSERT_FALSE(decoder.datagramReady());
        decoder.processByte(datagram[k++]);
    }

    // Datagram should be ready
    ASSERT_TRUE(decoder.datagramReady());
    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    *out = decoder.retrieveDatagram();

    // State machine should be reset
    ASSERT_FALSE(decoder.datagramReady());
    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

}

static void parseAndTestTelemetryPacket(Decoder &decoder, vector<uint8_t> &datagram, uint32_t timestamp,
                                        XYZReading accelReading, XYZReading magReading, XYZReading gyroReading,
                                        float temp, float pres, uint16_t pitot
) {

    Datagram d;
    parsePacket(decoder, datagram, PayloadType::TELEMETRY, &d);

    std::shared_ptr<TelemetryReading> data = std::dynamic_pointer_cast<TelemetryReading>(d.deserializedPayload_);
    EXPECT_EQ(timestamp, (*data).timestamp_);
    EXPECT_NEAR(accelReading.x_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.x_, epsilon);
    EXPECT_NEAR(accelReading.y_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.y_, epsilon);
    EXPECT_NEAR(accelReading.z_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.z_, epsilon);
    EXPECT_NEAR(gyroReading.x_, (*data).gyroscope_.x_, epsilon);
    EXPECT_NEAR(gyroReading.y_, (*data).gyroscope_.y_, epsilon);
    EXPECT_NEAR(gyroReading.z_, (*data).gyroscope_.z_, epsilon);
    EXPECT_NEAR(magReading.x_, (*data).magnetometer_.x_, epsilon);
    EXPECT_NEAR(magReading.y_, (*data).magnetometer_.y_, epsilon);
    EXPECT_NEAR(magReading.z_, (*data).magnetometer_.z_, epsilon);
    EXPECT_NEAR(temp, (*data).temperature_, epsilon);
    EXPECT_NEAR(pres / 100.0f, (*data).pressure_, epsilon);
    EXPECT_NEAR(altitudeFromPressure(pres / 100.0f), (*data).altitude_, epsilon);
    EXPECT_NEAR(airSpeedFromPitotPressure(pitot), (*data).air_speed_, epsilon);
}

TEST(DecoderTests, singleTelemetryPacketDecoding) {

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    // Test values
    uint32_t seqnum = 1410;
    uint32_t timestamp = 99999999;
    int16_t ax = -1;
    int16_t ay = 2;
    int16_t az = -3;
    int16_t mx = 4;
    int16_t my = -5;
    int16_t mz = 6;
    int16_t gx = -7;
    int16_t gy = 8;
    int16_t gz = -9;
    float temp = 12345.6789f;
    float pres = 1.41;
    uint16_t pitot = 5000;


    vector<uint8_t> datagram = createDatagram(seqnum, timestamp, ax, ay, az, mx, my, mz, gx, gy, gz, temp, pres, pitot);

    parseAndTestTelemetryPacket(decoder, datagram, timestamp,
                                {static_cast<double>(ax), static_cast<double>(ay), static_cast<double>(az)},
                                {static_cast<double>(mx), static_cast<double>(my), static_cast<double>(mz)},
                                {static_cast<double>(gx), static_cast<double>(gy), static_cast<double>(gz)}, temp, pres,
                                pitot);
}

TEST(DecoderTests, multipleConsecutiveTelemetryPacketsDecoding) {
    Decoder decoder{};
    srand(0);
    constexpr size_t measureCount = 10000;

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    // Test values
    std::array<uint32_t, measureCount> seqnum{}, timestamp{};
    std::array<int16_t, measureCount> ax{}, ay{}, az{}, mx{}, my{}, mz{}, gx{}, gy{}, gz{}, pitot{};
    std::array<float, measureCount> temp{}, pressure{};

    Rand<uint32_t> uint32Rand;
    Rand<int16_t> int16Rand;
    Rand<double> positiveDoubleRand(0, 1000000);
    Rand<double> doubleRand(-10000, 10000);

    for (size_t i = 0; i < measureCount; i++) {
        timestamp[i] = uint32Rand();
        seqnum[i] = uint32Rand();
        ax[i] = int16Rand();
        ay[i] = int16Rand();
        az[i] = int16Rand();
        mx[i] = int16Rand();
        my[i] = int16Rand();
        mz[i] = int16Rand();
        gx[i] = int16Rand();
        gy[i] = int16Rand();
        gz[i] = int16Rand();
        temp[i] = static_cast<float>(doubleRand());
        pressure[i] = static_cast<float>(positiveDoubleRand());
        pitot[i] = 5000 + i;
    }

    for (size_t i = 0; i < measureCount; i++) {
        vector<uint8_t> datagram = createDatagram(seqnum[i], timestamp[i], ax[i], ay[i], az[i], mx[i], my[i], mz[i],
                                                  gx[i], gy[i], gz[i], temp[i], pressure[i], pitot[i]);

        parseAndTestTelemetryPacket(decoder, datagram, timestamp[i],
                                    {static_cast<double>(ax[i]), static_cast<double>(ay[i]),
                                     static_cast<double>(az[i])},
                                    {static_cast<double>(mx[i]), static_cast<double>(my[i]),
                                     static_cast<double>(mz[i])},
                                    {static_cast<double>(gx[i]), static_cast<double>(gy[i]),
                                     static_cast<double>(gz[i])},
                                    temp[i], pressure[i], pitot[i]);
    }

}

TEST(DecoderTests, resistsToRandomByteSequence) {
    srand(0);
    const size_t randomTestSequenceLength = 10000;
    Decoder decoder{};

    for (int i = 0; i < randomTestSequenceLength; i++) {
        if (decoder.processByte(static_cast<uint8_t>(rand() % 256))) {
            decoder.retrieveDatagram();
        }
    }
    ASSERT_TRUE(true);
}

TEST(DecoderTests, resistsToRandomHeader) {
    srand(0);
    vector<uint8_t> byteSeq{};
    const size_t datagramLength = 1000;
    const size_t datagramCounts = 1000;
    Decoder decoder{};

    for (int i = 0; i < datagramCounts; i++) {
        feedWithValidPreamble(decoder);
        for (int j = 0; j < datagramLength; j++) {
            if (decoder.processByte(static_cast<uint8_t>(rand() % 256))) {
                decoder.retrieveDatagram();
            }
        }
    }

    ASSERT_TRUE(true);
}

TEST(DecoderTests, resistsToRandomPayloads) {
    vector<uint8_t> byteSeq{};
    const size_t datagramLength = 1000;
    const size_t datagramCounts = 10000;
    Decoder decoder{};

    for (int i = 0; i < datagramCounts; i++) {

        feedWithValidHeader(decoder);

        for (int j = 0; j < datagramLength; j++) {

            uint8_t randomByte = static_cast<uint8_t>(rand() % 256);

            // Avoids frame starts
            if (randomByte == HEADER_PREAMBLE_FLAG) {
                randomByte -= 1;
            }

            if (decoder.processByte(randomByte)) {
                decoder.retrieveDatagram();
            }
        }
    }


    ASSERT_TRUE(true);
}

TEST(DecoderTests, missingControlFlagResetsMachine) {
    srand(0);
    Decoder decoder{};

    for (int b = 0; b <= UINT8_MAX; b++) {
        if (b == CONTROL_FLAG) {
            continue;
        }

        EXPECT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

        feedWithValidPreamble(decoder);
        feedWithValidSequenceNumber(decoder);
        feedWithValidPayloadType(decoder);

        EXPECT_EQ(decoder.currentState(), DecodingState::SEEKING_CONTROL_FLAG);

        decoder.processByte(b);

        EXPECT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);
    }

}

TEST(DecoderTests, wrongChecksumDropsPacket) {
    Decoder decoder{};

    vector<uint8_t> datagram = createDatagram(1410, 999, -1, 2, -3, 4, -5, 6, -7, 8, -9, 12.34, 56.78, 0);
    vector<uint8_t> validDatagram = datagram;

    datagram[datagram.size() - 1] += 1;

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    for (uint8_t b : datagram) {
        ASSERT_FALSE(decoder.processByte(b));
    }

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    for (size_t i = 0; i < datagram.size(); i++) {
        if (i == datagram.size() - 1) {
            ASSERT_TRUE(decoder.processByte(validDatagram[i]));
        } else {
            ASSERT_FALSE(decoder.processByte(validDatagram[i]));
        }
    }

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);
    ASSERT_TRUE(decoder.datagramReady());

}

TEST(DecoderTests, SingleEventTest) {
    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    uint32_t seqnum{1234};
    uint32_t timestamp{18};
    int8_t code{0};

    vector<uint8_t> datagram = createEventDatagram(seqnum, timestamp, code);
    Datagram d{};

    parsePacket(decoder, datagram, PayloadType::EVENT, &d);

    std::shared_ptr<RocketEvent> data = std::dynamic_pointer_cast<RocketEvent>(d.deserializedPayload_);
    EXPECT_EQ(seqnum, d.sequenceNumber_);
    EXPECT_EQ(timestamp, data.get()->timestamp_);
    EXPECT_EQ(code, data.get()->code);
}

TEST(DecoderTests, MultipleEventPacketsTest) {

    size_t measureCount = 256 * 5;

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    Rand<uint32_t> uint32Rand;
    Rand<uint8_t> uint8Rand;

    for (size_t i = 0; i < measureCount; i++) {
        uint32_t timestamp = uint32Rand();
        uint32_t seqnum = uint32Rand();
        uint8_t code = uint8Rand();

        std::cout << static_cast<int>(code) << std::endl;

        vector<uint8_t> datagram = createEventDatagram(seqnum, timestamp, code);
        Datagram d{};

        parsePacket(decoder, datagram, PayloadType::EVENT, &d);

        std::shared_ptr<RocketEvent> data = std::dynamic_pointer_cast<RocketEvent>(d.deserializedPayload_);
        EXPECT_EQ(seqnum, d.sequenceNumber_);
        EXPECT_EQ(timestamp, (*data).timestamp_);
        if (RocketEventConstants::EVENT_CODES.find(code)
            != RocketEventConstants::EVENT_CODES.end()) {
            EXPECT_EQ(code, data.get()->code);
        } else {
            EXPECT_EQ(RocketEventConstants::INVALID_EVENT_CODE, data.get()->code);
        }
    }

}

TEST(DecoderTests, SingleControlPacketTest) {

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    uint32_t seqnum{1234};
    uint32_t timestamp{18};
    int8_t partCode{1};
    uint16_t statusValue{300};

    vector<uint8_t> datagram = createControlDatagram(seqnum, timestamp, partCode, statusValue);
    Datagram d{};

    parsePacket(decoder, datagram, PayloadType::CONTROL, &d);

    std::shared_ptr<ControlStatus> data = std::dynamic_pointer_cast<ControlStatus>(d.deserializedPayload_);
    EXPECT_EQ(seqnum, d.sequenceNumber_);
    EXPECT_EQ(timestamp, data.get()->timestamp_);
    EXPECT_EQ(partCode, data.get()->partCode_);
    EXPECT_EQ(statusValue, data.get()->statusValue_);

}


TEST(DecoderTests, MultipleControlPacketTest) {
    size_t measureCount = 256 * 5;

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    Rand<uint32_t> uint32Rand;
    Rand<uint16_t> uint16Rand;
    Rand<uint8_t> uint8Rand;

    for (size_t i = 0; i < measureCount; i++) {

        uint32_t seqnum = uint32Rand();
        uint32_t timestamp = uint32Rand();
        int8_t partCode = uint8Rand();
        uint16_t statusValue = uint16Rand();

        vector<uint8_t> datagram = createControlDatagram(seqnum, timestamp, partCode, statusValue);
        Datagram d{};

        parsePacket(decoder, datagram, PayloadType::CONTROL, &d);

        std::shared_ptr<ControlStatus> data = std::dynamic_pointer_cast<ControlStatus>(d.deserializedPayload_);
        EXPECT_EQ(seqnum, d.sequenceNumber_);
        EXPECT_EQ(timestamp, data.get()->timestamp_);

        if (ControlConstants::CONTROL_PARTS_CODES.find(partCode)
            != ControlConstants::CONTROL_PARTS_CODES.end()) {
            EXPECT_EQ(partCode, data.get()->partCode_);
            EXPECT_EQ(statusValue, data.get()->statusValue_);
        } else {
            EXPECT_EQ(ControlConstants::INVALID_PART_CODE, data.get()->partCode_);
            EXPECT_EQ(ControlConstants::INVALID_STATUS_VALUE, data.get()->statusValue_);
        }
    }

}