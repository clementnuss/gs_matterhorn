#ifndef GS_MATTERHORN_PROGRAMCONSTANTS_H
#define GS_MATTERHORN_PROGRAMCONSTANTS_H

#include <map>
#include <string>

namespace PrintConstants {
    static constexpr int PRECISION = 4;
    static constexpr int FIELD_WIDTH = 12;
    static constexpr char DELIMITER = ' ';
}

namespace LogConstants {
    static const std::string TELEMETRY_PATH{"telemetry_data"};
    static const std::string EVENTS_PATH{"events_data"};
}

namespace TimeConstants {
    static constexpr int MSECS_IN_SEC = 1000;
    static constexpr int SECS_IN_MINUTE = 60;
    static constexpr int MINUTES_IN_HOUR = 60;
    static constexpr int SECS_AND_MINS_WIDTH = 2;
    static constexpr int MSECS_WIDTH = 3;
}

namespace UIConstants {
    static constexpr int PRECISION = 2;

    // Refresh rates are in milliseconds
    static constexpr int NUMERICAL_VALUES_REFRESH_RATE = 100;
    static constexpr int REFRESH_RATE = 50;

    static constexpr int MSECS_GRAPH_XRANGE = 10000;
}

namespace SimulatorConstants {
    static constexpr size_t MAX_RANDOM_VECTOR_LENGTH = 16;
    static constexpr int EVENT_PROBABILITY_INTERVAL = 100;
}

namespace SensorConstants {
    static constexpr float MPU_ACCEL_MULTIPLIER = 1.0f / 208.77f;
}

namespace DataConstants {
    static constexpr double DELETION_FACTOR = 0.25;
    static constexpr int MAX_DATA_VECTOR_SIZE =
            (1.0 + DELETION_FACTOR) * (UIConstants::MSECS_GRAPH_XRANGE / UIConstants::REFRESH_RATE);
}

namespace CommunicationsConstants {
    static constexpr uint32_t TELEMETRY_BAUD_RATE = 115200;
}

static const std::map<int, std::string> EVENT_CODES = {
        {0, "Dummy event 1"},
        {1, "Dummy event 2"},
        {2, "Dummy event 3"},
        {3, "Dummy event 4"}
};

#endif //GS_MATTERHORN_PROGRAMCONSTANTS_H
