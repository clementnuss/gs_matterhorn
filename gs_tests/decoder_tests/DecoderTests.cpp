#include <DataHandlers/TelemetryHandler.h>
#include <gtest/gtest.h>
#include <DataHandlers/Receiver/Decoder.h>

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
                               uint32_t ax,
                               uint32_t ay,
                               uint32_t az,
                               uint32_t mx,
                               uint32_t my,
                               uint32_t mz,
                               uint32_t gx,
                               uint32_t gy,
                               uint32_t gz,
                               float temp,
                               uint32_t pres,
                               float alt) {
    // Create datagram header
    vector<uint8_t> datagram{
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG,
            HEADER_PREAMBLE_FLAG};

    for (int i = 3; i >= 0; --i)
        datagram.push_back(static_cast<uint8_t>(seqnum >> (8 * i)));

    datagram.push_back(static_cast<uint8_t>(DatagramPayloadType::TELEMETRY));
    datagram.push_back(CONTROL_FLAG);

    for (int i = 3; i >= 0; --i)
        datagram.push_back(static_cast<uint8_t>(timestamp >> (8 * i)));

    push3D(datagram, 2, ax, ay, az);
    push3D(datagram, 2, mx, my, mz);
    push3D(datagram, 2, gx, gy, gz);

    float_cast temperature{.fl = temp};
    for (int i = 3; i >= 0; --i)
        datagram.push_back(static_cast<uint8_t>(temperature.uint32 >> (8 * i)));


    float_cast pressure{.uint32 = pres};
    for (int i = 3; i >= 0; --i)
        datagram.push_back(static_cast<uint8_t>(pressure.uint32 >> (8 * i)));


    float_cast altitude{.fl= alt};
    for (int i = 3; i >= 0; --i)
        datagram.push_back(static_cast<uint8_t>(altitude.uint32 >> (8 * i)));


    return datagram;
}

TEST(DecoderTests, singlePacketDecoding) {

    Decoder dec = Decoder{};

    ASSERT_EQ(dec.currentState(), DecodingState::SEEKING_FRAMESTART);

    // Test values
    uint32_t seqnum = 1410;
    uint32_t timestamp = 99999999;
    int16_t ax = -1;
    int16_t ay = 2;
    int16_t az = 3;
    int16_t mx = 4;
    int16_t my = 5;
    int16_t mz = 6;
    int16_t gx = 7;
    int16_t gy = 8;
    int16_t gz = 9;
    float temp = 12345.6789f;
    uint32_t pres = 0141;
    float alt = 98765.3210f;


    vector<uint8_t> datagram = createDatagram(seqnum, timestamp, ax, ay, az, mx, my, mz, gx, gy, gz, temp, pres, alt);

    size_t k = 0;

    // Process preamble
    for (size_t i = 0; i < PREAMBLE_SIZE; i++) {
        ASSERT_EQ(dec.currentState(), DecodingState::SEEKING_FRAMESTART);
        ASSERT_FALSE(dec.datagramReady());
        dec.processByte(datagram[k++]);
    }

    // Process header
    for (size_t i = 0; i < HEADER_SIZE; i++) {
        ASSERT_EQ(dec.currentState(), DecodingState::PARSING_HEADER);
        ASSERT_FALSE(dec.datagramReady());
        dec.processByte(datagram[k++]);
    }

    // Process control flag
    ASSERT_EQ(dec.currentState(), DecodingState::SEEKING_CONTROL_FLAG);
    ASSERT_FALSE(dec.datagramReady());
    dec.processByte(datagram[k++]);

    // Process payload
    for (size_t i = 0; i < PAYLOAD_TYPES_LENGTH.at(DatagramPayloadType::TELEMETRY); i++) {
        ASSERT_EQ(dec.currentState(), DecodingState::PARSING_PAYLOAD);
        ASSERT_FALSE(dec.datagramReady());
        dec.processByte(datagram[k++]);
    }

    // Process checksum
    for (size_t i = 0; i < CHECKSUM_SIZE; i++) {
        ASSERT_EQ(dec.currentState(), DecodingState::PARSING_CHECKSUM);
        ASSERT_FALSE(dec.datagramReady());
        dec.processByte(datagram[k++]);
    }

    // Datagram should be ready
    ASSERT_TRUE(dec.datagramReady());
    ASSERT_EQ(dec.currentState(), DecodingState::SEEKING_FRAMESTART);

    Datagram d = dec.retrieveDatagram();

    // State machine should be reset
    ASSERT_FALSE(dec.datagramReady());
    ASSERT_EQ(dec.currentState(), DecodingState::SEEKING_FRAMESTART);

    std::shared_ptr<TelemetryReading> data = std::dynamic_pointer_cast<TelemetryReading>(d.deserializedPayload_);
    ASSERT_EQ(timestamp, (*data).timestamp);
    ASSERT_EQ(ax, (*data).acceleration_.x_);
    ASSERT_EQ(ay, (*data).acceleration_.y_);
    ASSERT_EQ(az, (*data).acceleration_.z_);
    ASSERT_EQ(mx, (*data).magnetometer_.x_);
    ASSERT_EQ(my, (*data).magnetometer_.y_);
    ASSERT_EQ(mz, (*data).magnetometer_.z_);
    ASSERT_EQ(gx, (*data).gyroscope_.x_);
    ASSERT_EQ(gy, (*data).gyroscope_.y_);
    ASSERT_EQ(gz, (*data).gyroscope_.z_);
}

TEST(DecoderTests, resistsToRandomByteSequence) {
    srand(0);
    const size_t randomTestSequenceLength = 10000;
    Decoder decoder{};

    for (int i = 0; i < randomTestSequenceLength; i++) {
        decoder.processByte(static_cast<uint8_t>(rand() % 256));
    }
}

TEST(DecoderTests, resistsToRandomDatagram) {
    srand(0);
    vector<uint8_t> byteSeq{};
    const size_t datagramLength = 100;
    const size_t datagramCounts = 10000;
    Decoder decoder{};

    for (int i = 0; i < datagramCounts; i++) {
        decoder.processByte(HEADER_PREAMBLE_FLAG);
        decoder.processByte(HEADER_PREAMBLE_FLAG);
        decoder.processByte(HEADER_PREAMBLE_FLAG);
        decoder.processByte(HEADER_PREAMBLE_FLAG);
        for (int j = 0; j < datagramLength; j++) {
            decoder.processByte(static_cast<uint8_t>(rand() % 256));
        }
    }

    ASSERT_TRUE(true);
}