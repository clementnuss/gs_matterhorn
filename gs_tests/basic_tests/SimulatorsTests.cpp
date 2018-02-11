#include <DataHandlers/TelemetryHandler.h>
#include <DataHandlers/Simulator/TelemetrySimulator.h>
#include "gtest/gtest.h"

TEST(SimulatorsTests, simulatedTelemetryIsCorrect) {

    TelemetrySimulator ts;
    TelemetryHandler &telemetryHandler = ts;

    for (int i = 0; i < 100; i++) {
        vector<SensorsPacket> data = ts.pollData();

        EXPECT_TRUE(data.size() >= 0);
        EXPECT_TRUE(data.size() <= SimulatorConstants::MAX_RANDOM_VECTOR_LENGTH);

        for (SensorsPacket tr : data) {
            std::cout << tr.toString() << std::endl;
        }
        //TODO: perform more elaborate tests
    }
}
