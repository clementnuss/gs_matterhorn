#ifndef GS_MATTERHORN_PROGRAMCONSTANTS_H
#define GS_MATTERHORN_PROGRAMCONSTANTS_H

#include <map>
#include <string>
#include "CRC/CRC.h"

#define DEBUG false

namespace PrintConstants {
    static constexpr int PRECISION = 4;
    static constexpr int FIELD_WIDTH = 12;
    static constexpr char DELIMITER = ' ';
}

namespace LogConstants {
    static const std::string WORKER_TELEMETRY_LOG_PATH{"telemetry_data"};
    static const std::string WORKER_EVENTS_LOG_PATH{"events_data"};
    static const std::string BYTES_LOG_PATH{"bytes_received"};
    static const std::string DECODER_LOG_PATH{"radio_receiver_events"};
}

namespace TimeConstants {
    static constexpr int MSECS_IN_SEC = 1000;
    static constexpr int SECS_IN_MINUTE = 60;
    static constexpr int MINUTES_IN_HOUR = 60;
    static constexpr int SECS_AND_MINS_WIDTH = 2;
    static constexpr int MSECS_WIDTH = 3;
}

namespace UserIfaceConstants {
    static constexpr int PRECISION = 2;

    // Refresh rates are in milliseconds
    static constexpr int NUMERICAL_VALUES_REFRESH_RATE = 100;
    static constexpr int REFRESH_RATE = 20; // 20ms = 50 FPS

    // Time in microseconds between each data point in the real-time graphs
    static constexpr int GRAPH_DATA_INTERVAL_USECS = 12'000;

    static constexpr float GRAPH_RANGE_MARGIN_RATIO = 1.15;
    static constexpr int GRAPH_XRANGE_SECS = 20;
}

namespace SimulatorConstants {
    static constexpr size_t MAX_RANDOM_VECTOR_LENGTH = 16;
    static constexpr int EVENT_PROBABILITY_INTERVAL = 100;
}

namespace SensorConstants {
    static constexpr float MPU_ACCEL_RANGE = 8.0f;
    static constexpr uint32_t ACCEL_SENSITIVITY = 32768;
    static constexpr float MPU_ACCEL_MULTIPLIER = MPU_ACCEL_RANGE / ACCEL_SENSITIVITY;

    /* SSCMRNN015PG5A3 0 - 15 psi*/
    static constexpr float PRESSURE_SENSOR2_MAX = 103421.f;    /* [Pa] */
    static constexpr float PRESSURE_SENSOR2_MIN = 0.f;         /* [Pa] */
    static constexpr float AIR_DENSITY = 1.225f;         /* [Pa] */


    // Values can be found here:
    // http://www.meteosuisse.admin.ch/home/meteo/valeurs-de-mesures/valeurs-de-mesures-aux-stations.html?param=airpressure-qfe
    static float currentLocationReferenceHPa = 968.1f;
    static float currentLocationHeight = 456.0f;
    static float currentLocationTemperature = 8.2f;
}

namespace DataConstants {
    static constexpr double DELETION_FACTOR = -0.05;
    static constexpr uint32_t READINGS_PER_SEC = 1'000'000 / UserIfaceConstants::GRAPH_DATA_INTERVAL_USECS;
    static constexpr int MAX_DATA_VECTOR_SIZE =
            static_cast<const int>((1.0 + DELETION_FACTOR) *
                                   (UserIfaceConstants::GRAPH_XRANGE_SECS * READINGS_PER_SEC));
}

namespace CommunicationsConstants {
    // See https://users.ece.cmu.edu/~koopman/crc/index.html for good polynomials
    static constexpr CRC::Parameters<crcpp_uint16, 16> CRC_16_GENERATOR_POLY = {0xA2EB, 0xFFFF, 0xFFFF, false, false};

    static constexpr int MSECS_BETWEEN_LINK_CHECKS = 0;
    static constexpr uint32_t TELEMETRY_BAUD_RATE = 115200;
    static constexpr int MSECS_NOMINAL_RATE = 200;
    static constexpr float MSECS_LOSSY_RATE = 500;
}

static const std::map<int, std::string> EVENT_CODES = {
        {0, "Dummy event 1"},
        {1, "Dummy event 2"},
        {2, "Dummy event 3"},
        {3, "Dummy event 4"}
};

#endif //GS_MATTERHORN_PROGRAMCONSTANTS_H
