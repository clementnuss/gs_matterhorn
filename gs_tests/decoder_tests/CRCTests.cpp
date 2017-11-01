#include <gtest/gtest.h>
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
    int numberOfTests = 10000;
    std::vector<uint8_t> originalSequence{};
    srand(0);

    for (size_t i = 0; i < 4; i++) {
        originalSequence.push_back(rand() % 256);
    }

    crcpp_uint16 originalCRC = CRC::Calculate(&originalSequence[0], originalSequence.size(),
                                              CommunicationsConstants::CRC_16_GENERATOR_POLY);

    for (int i = 0; i < numberOfTests; i++) {

        std::vector<uint8_t> sequence = originalSequence;
        size_t randomIndex = rand() % originalSequence.size();
        sequence[randomIndex] = rand() % 255;

        EXPECT_NE(originalCRC,
                  CRC::Calculate(&sequence[0], sequence.size(), CommunicationsConstants::CRC_16_GENERATOR_POLY));
    }
}

TEST(CRCTest, multipleByteChangesAreDetectedOnTelemetryPayloads) {
    ASSERT_TRUE(false);
}