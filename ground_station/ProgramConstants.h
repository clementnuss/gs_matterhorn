#ifndef GS_MATTERHORN_PROGRAMCONSTANTS_H
#define GS_MATTERHORN_PROGRAMCONSTANTS_H

#include <map>
#include <string>
#include "CRC/CRC.h"
#include <QVector3D>
#include <Qt3DRender/QLayer>
#include <3D/CoordinateUtils.h>

#define DEBUG false
#define USE_TRACKING false
#define USE_3D_MODULE true
#define TEST3D true

static const LatLon ORIGIN_3D_MODULE = {46.518732, 6.564058};
static const LatLon LAUNCH_SITE_LATLON = ORIGIN_3D_MODULE;
static const LatLon GS_LATLON = {46.518742, 6.564068};
static const std::string DEM_PATH_1 = "../../ground_station/data/N47E008.hgt";
static const std::string DEM_PATH_2 = "../../ground_station/data/N47E009.hgt";
static const GeoPoint DEM_TL_1{{47, 0, 0},
                               {8,  0, 0}};
static const GeoPoint DEM_TL_2{{47, 0, 0},
                               {9,  0, 0}};

namespace UnitsConstants {
    static constexpr float KNOTS_TO_MS = 0.514444;
}

namespace PredictorConstants {
    static constexpr float FAST_DESCENT_RATE = 25.0;
    static constexpr float SLOW_DESCENT_RATE = 5.0;
    static constexpr float PARACHUTE_DEPLOYMENT_ALTITUDE = 450.0;
    static constexpr float WIND_DRAG_FACTOR = 1.0;
    static constexpr int PREDICTION_TIME_INTERVAL = 1;
}

namespace FileConstants {
    static constexpr int WIND_PREDICTIONS_HEADER_LINES_COUNT = 4;
}

namespace PrintConstants {
    static constexpr int PRECISION = 4;
    static constexpr int FIELD_WIDTH = 12;
    static constexpr char DELIMITER = ' ';
}

namespace LogConstants {
    static const std::string WORKER_TELEMETRY_LOG_PATH{"sensors_data"};
    static const std::string WORKER_EVENTS_LOG_PATH{"events_data"};
    static const std::string WORKER_GPS_LOG_PATH{"gps_data"};
    static const std::string BYTES_LOG_PATH{"bytes_received"};
    static const std::string DECODER_LOG_PATH{"radio_receiver_events"};
}

namespace TimeConstants {
    static constexpr int MSECS_IN_SEC = 1000;
    static constexpr int USECS_IN_SEC = 1000 * MSECS_IN_SEC;
    static constexpr int SECS_IN_MINUTE = 60;
    static constexpr int MINUTES_IN_HOUR = 60;
    static constexpr int SECS_AND_MINS_WIDTH = 2;
    static constexpr int MSECS_WIDTH = 3;
}

namespace UIConstants {
    static constexpr int PRECISION = 2;
    static constexpr int PRECISION_GPS = 6;

    // Refresh rates are in milliseconds
    static constexpr int NUMERICAL_SENSORS_VALUES_REFRESH_RATE = 100;
    static constexpr int REFRESH_RATE = 20; // 20ms = 50 FPS

    // Time in microseconds between each data point in the real-time graphs
    static constexpr int GRAPH_DATA_INTERVAL_USECS = 12000;

    static constexpr float GRAPH_RANGE_MARGIN_RATIO = 1.15;
    static constexpr int GRAPH_XRANGE_SECS = 20;
    static constexpr int GRAPH_MEMORY_SECS = 600;
    static constexpr int GRAPH_MEMORY_USECS = GRAPH_MEMORY_SECS * TimeConstants::USECS_IN_SEC;
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
    /*
    static float currentLocationReferenceHPa = 964.9f;
    static float currentLocationHeight = 456.0f;
    static float currentLocationTemperature = 1.6f;
     */

    static float adjustedSeaLevelPressure = 1028.6;
    static float launchAltitude = 408.3;
    static float trackingAltitude = 409;
    static float distanceToLaunchSite = 0.9;

}

namespace DataConstants {
    static constexpr double INCREASE_FACTOR = 1.5;
    static constexpr double DECREASE_FACTOR = 1 / INCREASE_FACTOR;
    static constexpr uint32_t READINGS_PER_SEC = 1'000'000 / UIConstants::GRAPH_DATA_INTERVAL_USECS;
}

namespace CommunicationsConstants {

    /**
     * CRC 16 bits:
     * 0xd175 in implicit +1 notation
     * 0xA2EB in explicit +1 notation
     *
     * Hamming Distance of 4 up to 32751 bits dataword
     *
     * Chosen according to the guidelines of Philip Koopman from Carnegie Mellon University
     * which performed tests on a wide range of polynomials.
     *
     * See https://users.ece.cmu.edu/~koopman/crc/index.html
     */
    static constexpr CRC::Parameters<crcpp_uint16, 16> CRC_16_GENERATOR_POLY = {0xA2EB, 0xFFFF, 0xFFFF, false, false};

    static constexpr int MSECS_BETWEEN_LINK_CHECKS = 0;
    static constexpr uint32_t TELEMETRY_BAUD_RATE = 115200;
    static constexpr int MSECS_NOMINAL_RATE = 200;
    static constexpr float MSECS_LOSSY_RATE = 500;

    static constexpr int TELEMETRY_TYPE = 0;
    static constexpr int EVENT_TYPE = 1;
    static constexpr int CONTROL_TYPE = 2;
    static constexpr int GPS_TYPE = 3;
}

//TODO: use uint8 ?
namespace RocketEventConstants {

    static constexpr int INVALID_EVENT_CODE = 0;

    static const std::map<int, std::string> EVENT_CODES = {
            {1,  "Liftoff"},
            {10, "Burnout"},
            {20, "Max G"},
            {30, "Max V"},
            {40, "Apogee"},
            {50, "Drogue deploy"},
            {60, "Main deploy"},
            {70, "Touchdown"}
    };
}


namespace ControlConstants {

    static constexpr uint8_t INVALID_PART_CODE = 0;
    static constexpr uint16_t INVALID_STATUS_VALUE = 0;
    static const std::map<uint8_t, std::string> CONTROL_PARTS_CODES = {
            {1, "Airbrake"}
    };

}

#endif //GS_MATTERHORN_PROGRAMCONSTANTS_H
