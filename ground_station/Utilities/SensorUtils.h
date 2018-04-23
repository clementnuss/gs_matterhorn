
#ifndef GS_MATTERHORN_SENSORUTILS_H
#define GS_MATTERHORN_SENSORUTILS_H

#include <ProgramConstants.h>
#include <cmath>
#include <ConfigParser/ConfigParser.h>

static double altitudeFromPressure(float pressure_hPa) {
    return 44330 *
           (1.0 - pow(pressure_hPa / ConfSingleton::instance().get("adjustedSeaLevelPressure", 1000.0f), 0.1903));
}

static double airSpeedFromPitotPressure(uint16_t pitotPressure) {
    double p_press =
            ((static_cast<float>(pitotPressure)) - 1652) *
            (SensorConstants::PRESSURE_SENSOR2_MAX - SensorConstants::PRESSURE_SENSOR2_MIN) /
            (14745 - 1652) +
            SensorConstants::PRESSURE_SENSOR2_MIN;

    return sqrt(2 * p_press / SensorConstants::AIR_DENSITY);
}

#endif //GS_MATTERHORN_SENSORUTILS_H
