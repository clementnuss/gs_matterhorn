#include <DataHandlers/TelemetryHandler.h>
#include <gtest/gtest.h>
#include <DataHandlers/Receiver/Decoder.h>
#include <Utilities/RandUtils.h>

static constexpr double epsilon = 1e-4;

void push3D(std::vector<uint8_t> &v, size_t byteCount, uint32_t a, uint32_t b, uint32_t c) {
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(a >> (8 * i)));
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(b >> (8 * i)));
    for (int i = byteCount - 1; i >= 0; --i)
        v.push_back(static_cast<uint8_t>(c >> (8 * i)));
}

vector<uint8_t> createDatagram(uint32_t seqnum,
                               uint32_t timestamp,
                               int16_t ax,
                               int16_t ay,
                               int16_t az,
                               int16_t mx,
                               int16_t my,
                               int16_t mz,
                               int16_t gx,
                               int16_t gy,
                               int16_t gz,
                               float temp,
                               float pres,
                               float alt) {
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

    datagram.push_back(static_cast<uint8_t>(DatagramPayloadType::TELEMETRY));
    checksumAccumulator.push_back(static_cast<uint8_t>(DatagramPayloadType::TELEMETRY));
    datagram.push_back(CONTROL_FLAG);

    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));
    }

    push3D(datagram, 2, ax, ay, az);
    push3D(checksumAccumulator, 2, ax, ay, az);
    push3D(datagram, 2, mx, my, mz);
    push3D(checksumAccumulator, 2, mx, my, mz);
    push3D(datagram, 2, gx, gy, gz);
    push3D(checksumAccumulator, 2, gx, gy, gz);

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


    float_cast altitude{.fl= alt};
    for (int i = 3; i >= 0; --i) {
        datagram.push_back(static_cast<uint8_t>(altitude.uint32 >> (8 * i)));
        checksumAccumulator.push_back(static_cast<uint8_t>(altitude.uint32 >> (8 * i)));
    }

    checksum_t crc = CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                    CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = sizeof(checksum_t) - 1; i >= 0; i--) {
        datagram.push_back(static_cast<uint8_t>(crc >> (8 * i)));
    }

    return datagram;
}


void feedWithValidPreamble(Decoder &decoder) {
    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    for (int k = 0; k < PREAMBLE_SIZE; k++) {
        decoder.processByte(HEADER_PREAMBLE_FLAG);
    }

    ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_HEADER);
}

void feedWithValidSequenceNumber(Decoder &decoder) {
    decoder.processByte(0x01);
    decoder.processByte(0x01);
    decoder.processByte(0x01);
    decoder.processByte(0x01);
}

void feedWithValidPayloadType(Decoder &decoder) {
    // Select randomly one of the datagram payload types
    decoder.processByte(rand() % static_cast<int>(DatagramPayloadType::Count));
}

void feedWithValidHeader(Decoder &decoder) {

    feedWithValidPreamble(decoder);
    feedWithValidSequenceNumber(decoder);
    feedWithValidPayloadType(decoder);

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_CONTROL_FLAG);

    decoder.processByte(CONTROL_FLAG);

    ASSERT_EQ(decoder.currentState(), DecodingState::PARSING_PAYLOAD);
}

void parseAndTestPacket(Decoder &decoder, vector<uint8_t> &datagram, uint32_t timestamp,
                        XYZReading accelReading, XYZReading magReading, XYZReading gyroReading,
                        float temp, float pres, float alt
) {
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
    for (size_t i = 0; i < PAYLOAD_TYPES_LENGTH.at(DatagramPayloadType::TELEMETRY); i++) {
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

    Datagram d = decoder.retrieveDatagram();

    // State machine should be reset
    ASSERT_FALSE(decoder.datagramReady());
    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    std::shared_ptr<TelemetryReading> data = std::dynamic_pointer_cast<TelemetryReading>(d.deserializedPayload_);
    EXPECT_EQ(timestamp / 1000, (*data).timestamp_);
    EXPECT_NEAR(accelReading.x_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.x_, epsilon);
    EXPECT_NEAR(accelReading.y_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.y_, epsilon);
    EXPECT_NEAR(accelReading.z_ * SensorConstants::MPU_ACCEL_MULTIPLIER, (*data).acceleration_.z_, epsilon);
    EXPECT_NEAR(magReading.x_, (*data).magnetometer_.x_, epsilon);
    EXPECT_NEAR(magReading.y_, (*data).magnetometer_.y_, epsilon);
    EXPECT_NEAR(magReading.z_, (*data).magnetometer_.z_, epsilon);
    EXPECT_NEAR(gyroReading.x_, (*data).gyroscope_.x_, epsilon);
    EXPECT_NEAR(gyroReading.y_, (*data).gyroscope_.y_, epsilon);
    EXPECT_NEAR(gyroReading.z_, (*data).gyroscope_.z_, epsilon);
    EXPECT_NEAR(temp, (*data).temperature_, epsilon);
    EXPECT_NEAR(pres, (*data).pressure_, epsilon);
    EXPECT_NEAR(alt, (*data).altitude_, epsilon);
}

TEST(DecoderTests, singlePacketDecoding) {

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
    float alt = 98765.3210f;


    vector<uint8_t> datagram = createDatagram(seqnum, timestamp, ax, ay, az, mx, my, mz, gx, gy, gz, temp, pres, alt);

    parseAndTestPacket(decoder, datagram, timestamp,
                       {static_cast<double>(ax), static_cast<double>(ay), static_cast<double>(az)},
                       {static_cast<double>(mx), static_cast<double>(my), static_cast<double>(mz)},
                       {static_cast<double>(gx), static_cast<double>(gy), static_cast<double>(gz)}, temp, pres, alt);
}

TEST(DecoderTests, multipleConsecutivePacketsDecoding) {
    Decoder decoder{};
    srand(0);
    constexpr size_t measureCount = 10000;

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    // Test values
    std::array<uint32_t, measureCount> seqnum{}, timestamp{};
    std::array<int16_t, measureCount> ax{}, ay{}, az{}, mx{}, my{}, mz{}, gx{}, gy{}, gz{};
    std::array<float, measureCount> temp{}, pressure{}, alt{};

    Rand<uint32_t> uint32Rand;
    Rand<int16_t> int16Rand;
    Rand<double> doubleRand(-FLT_MAX, FLT_MAX);

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
        pressure[i] = static_cast<float>(doubleRand());
        alt[i] = static_cast<float>(doubleRand());
    }

    for (size_t i = 0; i < measureCount; i++) {
        vector<uint8_t> datagram = createDatagram(seqnum[i], timestamp[i],
                                                  ax[i], ay[i], az[i],
                                                  mx[i], my[i], mz[i],
                                                  gx[i], gy[i], gz[i],
                                                  temp[i], pressure[i], alt[i]);

        parseAndTestPacket(decoder, datagram, timestamp[i],
                           {static_cast<double>(ax[i]), static_cast<double>(ay[i]), static_cast<double>(az[i])},
                           {static_cast<double>(mx[i]), static_cast<double>(my[i]), static_cast<double>(mz[i])},
                           {static_cast<double>(gx[i]), static_cast<double>(gy[i]), static_cast<double>(gz[i])},
                           temp[i], pressure[i], alt[i]);
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
    srand(0);
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

        feedWithValidPreamble(decoder);
        feedWithValidSequenceNumber(decoder);
        feedWithValidPayloadType(decoder);

        ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_CONTROL_FLAG);

        decoder.processByte(b);

        ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);
    }

}

TEST(DecoderTests, wrongChecksumDropsPacket) {
    //TODO: Checks that machine is correctly reset after invalid checksum
    ASSERT_TRUE(false);
}