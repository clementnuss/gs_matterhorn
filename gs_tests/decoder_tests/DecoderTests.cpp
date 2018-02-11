#include <DataHandlers/TelemetryHandler.h>
#include <gtest/gtest.h>
#include <DataHandlers/Receiver/Decoder.h>
#include <Utilities/RandUtils.h>
#include <Utilities/SensorUtils.h>

static constexpr double epsilon = 1e-3;


// ------------------------------ Helper functions for pushing values in datagram vector ---------------------------- //
template<class T>
static void pushInteger(std::vector<uint8_t> *datagram,
                        std::vector<uint8_t> *checksumAccumulator,
                        T val) {
    for (int i = sizeof(T) - 1; i >= 0; --i) {
        datagram->push_back(static_cast<uint8_t>(val >> (8 * i)));
        checksumAccumulator->push_back(static_cast<uint8_t>(val >> (8 * i)));
    }
}

template<class T>
static void pushIntegers3D(std::vector<uint8_t> *datagram,
                           std::vector<uint8_t> *checksumAccumulator,
                           T a, T b, T c) {
    pushInteger(datagram, checksumAccumulator, a);
    pushInteger(datagram, checksumAccumulator, b);
    pushInteger(datagram, checksumAccumulator, c);
}

static void pushFloat(std::vector<uint8_t> *datagram,
                      std::vector<uint8_t> *checksumAccumulator,
                      float val) {
    float_cast fl_val{.fl = val};
    for (int i = sizeof(float) - 1; i >= 0; --i) {
        datagram->push_back(static_cast<uint8_t>(fl_val.uint32 >> (8 * i)));
        checksumAccumulator->push_back(static_cast<uint8_t>(fl_val.uint32 >> (8 * i)));
    }
}

static void pushChecksum(std::vector<uint8_t> *datagram, checksum_t crc) {
    for (int i = sizeof(checksum_t) - 1; i >= 0; i--) {
        datagram->push_back(static_cast<uint8_t>(crc >> (8 * i)));
    }
}


// ---------------------------------- Helper functions for creating datagrams  -------------------------------------- //
static void fillHeaderAndTimestamp(std::vector<uint8_t> *datagram,
                                   std::vector<uint8_t> *checksumAccumulator,
                                   uint32_t seqnum,
                                   PayloadType type,
                                   uint32_t timestamp) {
    // Create datagram header
    datagram->push_back(HEADER_PREAMBLE_FLAG);
    datagram->push_back(HEADER_PREAMBLE_FLAG);
    datagram->push_back(HEADER_PREAMBLE_FLAG);
    datagram->push_back(HEADER_PREAMBLE_FLAG);

    pushInteger(datagram, checksumAccumulator, seqnum);
    pushInteger(datagram, checksumAccumulator, static_cast<uint8_t>(type));

    datagram->push_back(CONTROL_FLAG);

    pushInteger(datagram, checksumAccumulator, timestamp);
}


static vector<uint8_t>
createDatagram(uint32_t seqnum, uint32_t timestamp, int16_t ax, int16_t ay, int16_t az, int16_t mx, int16_t my,
               int16_t mz, int16_t gx, int16_t gy, int16_t gz, float temp, float pres, int16_t pitot) {

    std::vector<uint8_t> datagram{};
    std::vector<uint8_t> checksumAccumulator{};

    fillHeaderAndTimestamp(&datagram, &checksumAccumulator, seqnum, PayloadType::TELEMETRY, timestamp);

    pushIntegers3D(&datagram, &checksumAccumulator, ax, ay, az);
    pushIntegers3D(&datagram, &checksumAccumulator, gx, gy, gz);
    pushIntegers3D(&datagram, &checksumAccumulator, mx, my, mz);
    pushFloat(&datagram, &checksumAccumulator, temp);
    pushFloat(&datagram, &checksumAccumulator, pres);
    pushInteger(&datagram, &checksumAccumulator, pitot);
    pushChecksum(&datagram, CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                           CommunicationsConstants::CRC_16_GENERATOR_POLY));

    return datagram;
}


static vector<uint8_t>
createEventDatagram(uint32_t seqnum, uint32_t timestamp, uint8_t code) {

    std::vector<uint8_t> datagram{};
    std::vector<uint8_t> checksumAccumulator{};

    fillHeaderAndTimestamp(&datagram, &checksumAccumulator, seqnum, PayloadType::EVENT, timestamp);

    pushInteger(&datagram, &checksumAccumulator, code);
    pushChecksum(&datagram, CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                           CommunicationsConstants::CRC_16_GENERATOR_POLY));

    return datagram;
}

static vector<uint8_t>
createControlDatagram(uint32_t seqnum, uint32_t timestamp, int8_t partCode, uint16_t statusValue) {

    std::vector<uint8_t> datagram{};
    std::vector<uint8_t> checksumAccumulator{};

    fillHeaderAndTimestamp(&datagram, &checksumAccumulator, seqnum, PayloadType::CONTROL, timestamp);

    pushInteger(&datagram, &checksumAccumulator, partCode);
    pushInteger(&datagram, &checksumAccumulator, statusValue);
    pushChecksum(&datagram, CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                           CommunicationsConstants::CRC_16_GENERATOR_POLY));

    return datagram;
}

static vector<uint8_t>
createGPSDatagram(uint32_t seqnum, uint32_t timestamp, uint8_t satCount, float rssi, float latitude, float longitude,
                  float altitude) {

    std::vector<uint8_t> datagram{};
    std::vector<uint8_t> checksumAccumulator{};

    fillHeaderAndTimestamp(&datagram, &checksumAccumulator, seqnum, PayloadType::GPS, timestamp);

    datagram.emplace_back(satCount);
    checksumAccumulator.emplace_back(satCount);

    pushFloat(&datagram, &checksumAccumulator, rssi);
    pushFloat(&datagram, &checksumAccumulator, latitude);
    pushFloat(&datagram, &checksumAccumulator, longitude);
    pushFloat(&datagram, &checksumAccumulator, altitude);

    pushChecksum(&datagram, CRC::Calculate(&checksumAccumulator[0], checksumAccumulator.size(),
                                           CommunicationsConstants::CRC_16_GENERATOR_POLY));

    return datagram;
}


// -------------------------- Helper functions for reaching specific Decoder states  ------------------------------- //
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


// ------------------------------------- Helper functions for testing payloads -------------------------------------- //
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




// ------------------------------------------------ Unit tests ------------------------------------------------------ //
TEST(DecoderTests, singleTelemetryPacket) {

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

TEST(DecoderTests, multipleTelemetryPackets) {
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
    Rand<double> positiveDoubleRand(0.0, 1000000.0);
    Rand<double> doubleRand(-10000.0, 10000.0);

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

TEST(DecoderTests, singleEventPacket) {
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

TEST(DecoderTests, multipleEventPackets) {

    size_t measureCount = 256 * 5;

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    Rand<uint32_t> uint32Rand;
    Rand<uint8_t> uint8Rand;

    for (size_t i = 0; i < measureCount; i++) {
        uint32_t timestamp = uint32Rand();
        uint32_t seqnum = uint32Rand();
        uint8_t code = uint8Rand();

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

TEST(DecoderTests, singleControlPacket) {

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


TEST(DecoderTests, multipleControlPackets) {
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

TEST(DecoderTests, singleGPSPacket) {

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    uint32_t seqnum{1234};
    uint32_t timestamp{18};

    uint8_t satsCount{1};
    float rssi{1234};
    float latitude{12.4587};
    float longitude{45.4537};
    float altitude{123.4167};

    vector<uint8_t> datagram = createGPSDatagram(seqnum, timestamp, satsCount, rssi, latitude, longitude, altitude);
    Datagram d{};

    parsePacket(decoder, datagram, PayloadType::GPS, &d);

    std::shared_ptr<GPSPacket> data = std::dynamic_pointer_cast<GPSPacket>(d.deserializedPayload_);
    EXPECT_EQ(seqnum, d.sequenceNumber_);
    EXPECT_EQ(timestamp, data.get()->timestamp_);
    EXPECT_EQ(satsCount, data.get()->satsCount_);
    EXPECT_NEAR(rssi, data.get()->rssi_, epsilon);
    EXPECT_NEAR(latitude, data.get()->latitude_, epsilon);
    EXPECT_NEAR(longitude, data.get()->longitude_, epsilon);
    EXPECT_NEAR(altitude, data.get()->altitude_, epsilon);
}


TEST(DecoderTests, multipleGPSPackets) {
    size_t measureCount = 256 * 5;

    Decoder decoder{};

    ASSERT_EQ(decoder.currentState(), DecodingState::SEEKING_FRAMESTART);

    Rand<uint32_t> uint32Rand;
    Rand<uint8_t> uint8Rand;
    Rand<float> floatRand(-100000.0f, 100000.0f);

    for (size_t i = 0; i < measureCount; i++) {

        uint32_t seqnum = uint32Rand();
        uint32_t timestamp = uint32Rand();
        uint8_t satsCount = uint8Rand();
        float rssi = floatRand();
        float latitude = floatRand();
        float longitude = floatRand();
        float altitude = floatRand();

        vector<uint8_t> datagram = createGPSDatagram(seqnum, timestamp, satsCount, rssi, latitude, longitude, altitude);
        Datagram d{};

        parsePacket(decoder, datagram, PayloadType::GPS, &d);

        std::shared_ptr<GPSPacket> data = std::dynamic_pointer_cast<GPSPacket>(d.deserializedPayload_);
        EXPECT_EQ(seqnum, d.sequenceNumber_);
        EXPECT_EQ(timestamp, data.get()->timestamp_);
        EXPECT_EQ(satsCount, data.get()->satsCount_);
        EXPECT_NEAR(rssi, data.get()->rssi_, epsilon);
        EXPECT_NEAR(latitude, data.get()->latitude_, epsilon);
        EXPECT_NEAR(longitude, data.get()->longitude_, epsilon);
        EXPECT_NEAR(altitude, data.get()->altitude_, epsilon);
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