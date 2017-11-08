#include <gtest/gtest.h>


static constexpr uint8_t XBEE_START = 0x7e;
static constexpr uint8_t XBEE_TX_FRAME = 0x10;
static constexpr uint8_t XBEE_BCAST_ADDR[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};

TEST(XBeeAPI, generateFrame) {

    uint8_t samplePayload[] = {0x55, 0x55, 0x55, 0x55, 0x00, 0x00, 0x05, 0xFF, 0x00, 0xFF, 0x5B,
                               0xB5, 0xB2, 0x70, 0x00, 0x27, 0xF7, 0x48, 0xF2, 0x50, 0x00, 0x11,
                               0x00, 0x19, 0x00, 0x0B, 0x00, 0x02, 0x00, 0x2B, 0xFF, 0xB9, 0x41,
                               0xD7, 0x70, 0xA4, 0x47, 0xBE, 0x99, 0x9A, 0x00, 0x00, 0x00, 0x00, 0x37, 0xD0};


    std::vector<uint8_t> apiPacket;

    uint8_t checksum = 0x00;

    apiPacket.push_back(XBEE_START);

    //Length = 14 + Payload size = 60
    apiPacket.push_back(0x00);
    apiPacket.push_back(0x3c);

    apiPacket.push_back(XBEE_TX_FRAME);
    checksum += XBEE_TX_FRAME;

    apiPacket.push_back(0x00);

    for (uint8_t byte : XBEE_BCAST_ADDR) {
        apiPacket.push_back(byte);
        checksum += byte;
    }

    // Reserved
    apiPacket.push_back(0xff);
    apiPacket.push_back(0xfe);
    checksum += 0xff;
    checksum += 0xfe;

    // Broadcast radius (0 = max)
    apiPacket.push_back(0x00);

    //Transmit options (disable ACK and Route discovery)
    apiPacket.push_back(0x03);
    checksum += 0x03;

    //Payload data
    for (uint8_t byte: samplePayload) {
        apiPacket.push_back(byte);
        checksum += byte;
    }

    apiPacket.push_back(static_cast<uint8_t >(0xff) - checksum);

    std::cout << std::endl << "API mode 1 (without escape)" << std::endl;
    for (uint8_t byte: apiPacket) {
        std::cout << " " << std::setw(2) << std::setfill('0') << std::hex << +byte;
    }

//    std::cout << std::endl << "API mode 2 (with escape)" << std::endl;

    std::cout << std::endl;

    ASSERT_EQ(0x0, 0);
}