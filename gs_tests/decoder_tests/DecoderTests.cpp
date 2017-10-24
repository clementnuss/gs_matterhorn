#include <DataHandlers/TelemetryHandler.h>
#include <gtest/gtest.h>
#include <DataHandlers/Receiver/Decoder.h>

TEST(DecoderTests, simulateNormalDecoding) {

    Decoder dec = Decoder{};

    ASSERT_EQ(dec.getCurrentState_(), DecodingState::SEEKING_FRAMESTART);

    vector<uint8_t> correctTransmission(
            {HEADER_PREAMBLE_FLAG, HEADER_PREAMBLE_FLAG, HEADER_PREAMBLE_FLAG, HEADER_PREAMBLE_FLAG, // header preamble
             0x00, 0x00, 0x05, 0x82, // Sequence number
             static_cast<uint8_t> (DatagramPayloadType::TELEMETRY)
            });

    for (uint8_t byte : correctTransmission)
        dec.processByte(byte);

    ASSERT_EQ(dec.getCurrentState_(), DecodingState::SEEKING_CONTROL_FLAG);

    dec.processByte(CONTROL_FLAG);

    ASSERT_EQ(dec.getCurrentState_(), DecodingState::PARSING_PAYLOAD);

    // Timestamp
    dec.processByte(0x80);
    dec.processByte(0x00);
    dec.processByte(0x00);
    dec.processByte(0x00);

    // Ax, ay, az, mx, my, mz, gx, gy, gz

    for (uint8_t i = 0; i < 9; ++i) {
        dec.processByte(0x01);
        dec.processByte(i);
    }

    float temperature_f = -14.14f;
    float_cast temp = {.fl = temperature_f};

    for (int i = 3; i >= 0; --i) {
        dec.processByte(static_cast<uint8_t>(temp.uint32 >> (8 * i)));
    }

    float_cast pressure = {.uint32 = 0xff7fffff};
    for (int i = 3; i >= 0; --i) {
        dec.processByte(static_cast<uint8_t>(pressure.uint32 >> (8 * i)));
    }

    float_cast altitude = {.fl= 3.4028235E38f};
    for (int i = 3; i >= 0; --i) {
        dec.processByte(static_cast<uint8_t>(altitude.uint32 >> (8 * i)));
    }

    ASSERT_TRUE(dec.datagramReady());

    Datagram datagram = dec.retrieveDatagram();
    ASSERT_EQ(datagram.sequenceNumber, 1410);

    ASSERT_EQ(datagram.payload.timestamp, 0x80000000);
    ASSERT_EQ(datagram.payload.altitude.value, 3.4028235E38f);

}
