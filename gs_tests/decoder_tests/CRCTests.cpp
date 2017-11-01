#include <gtest/gtest.h>
#include <Utilities/RandUtils.h>
#include "CRC/CRC.h"
#include "ProgramConstants.h"
#include "DataHandlers/Receiver/DatagramSpec.h"


TEST(CRCTest, CRC32IsCorrect) {

    const char helloWorld[] = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D'};
    const char fooBar[] = {'F', 'o', 'o', 'B', 'a', 'r'};

    std::vector<uint8_t> test{'a', 'b', 'c'};

    ASSERT_EQ(0x87E5865B, CRC::Calculate(helloWorld, sizeof(helloWorld), CRC::CRC_32()));
    ASSERT_EQ(0xA1175C43, CRC::Calculate(fooBar, sizeof(fooBar), CRC::CRC_32()));
    ASSERT_EQ(0x352441C2, CRC::Calculate(&test[0], 3, CRC::CRC_32()));

}

TEST(CRCTest, bitChangeIsDetected) {
    const char validSequence[] = {'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D'};
    const char invalidSequence[] = {'h', 'F', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D'};

    ASSERT_EQ(CRC::Calculate(validSequence, sizeof(validSequence), CommunicationsConstants::CRC_16_GENERATOR_POLY),
              CRC::Calculate(validSequence, sizeof(validSequence), CommunicationsConstants::CRC_16_GENERATOR_POLY));

    ASSERT_NE(CRC::Calculate(invalidSequence, sizeof(invalidSequence), CommunicationsConstants::CRC_16_GENERATOR_POLY),
              CRC::Calculate(validSequence, sizeof(validSequence), CommunicationsConstants::CRC_16_GENERATOR_POLY));

}

TEST(CRCTest, byteChangesAreDetectedOnTelemetryPayloads) {
    int numberOfTests = 100000;
    std::vector<uint8_t> originalSequence{};
    srand(0);

    for (size_t i = 0; i < PAYLOAD_TYPES_LENGTH.at(DatagramPayloadType::TELEMETRY); i++) {
        originalSequence.push_back(rand() % 256);
    }

    crcpp_uint16 originalCRC = CRC::Calculate(&originalSequence[0], originalSequence.size(),
                                              CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = 0; i < numberOfTests; i++) {

        std::vector<uint8_t> sequence = originalSequence;
        size_t randomIndex = rand() % originalSequence.size();
        uint8_t newByte = rand() % 255;

        if (newByte != sequence[randomIndex]) {
            sequence[randomIndex] = newByte;
        } else {
            sequence[randomIndex] += 1;
        }

        EXPECT_NE(originalCRC,
                  CRC::Calculate(&sequence[0], sequence.size(), CommunicationsConstants::CRC_16_GENERATOR_POLY));
    }
}

TEST(CRCTest, randomCollisionsOnUniformTelemetryPayloadsAreLow) {
    int numberOfTests = 500'000;

    Rand<uint8_t> uniformByteRand{};
    std::vector<uint8_t> seq1{};
    std::vector<uint8_t> seq2{};

    int pass{0};
    int fail{0};

    for (int i = 0; i < numberOfTests; i++) {

        seq1.clear();
        seq2.clear();
        for (size_t i = 0; i < PAYLOAD_TYPES_LENGTH.at(DatagramPayloadType::TELEMETRY) + 5; i++) {
            seq1.push_back(uniformByteRand());
            seq2.push_back(uniformByteRand());
        }

        if (seq1 == seq2) {
            continue;
        }

        uint32_t crc1 = CRC::Calculate(&seq1[0], seq1.size(), CommunicationsConstants::CRC_16_GENERATOR_POLY);
        uint32_t crc2 = CRC::Calculate(&seq2[0], seq2.size(), CommunicationsConstants::CRC_16_GENERATOR_POLY);

        if (crc1 == crc2) {
            fail++;
        } else {
            pass++;
        }
    }

    double ratio = fail / static_cast<double>(pass);
    std::cout << "Fail: " << fail << " Pass: " << pass << " Ratio: " << ratio << std::endl;
    ASSERT_TRUE(ratio <= 1.5e-5);
}

TEST(CRCTest, multipleByteChangesAreDetectedOnTelemetryPayloads) {
    ASSERT_TRUE(false);
}