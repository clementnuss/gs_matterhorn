#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include "ILoggable.h"

struct TimedData {
    TimedData(long t) : timestamp{t} {}

    long timestamp;
};

struct DataReading {
    DataReading(double v, bool b) : value{v}, validity{b} {}

    double value;
    bool validity;
};

struct YawPitchRollReading : ILoggable {
    YawPitchRollReading(double y, double p, double r, bool b) : yaw{y}, pitch{p}, roll{r}, validity{b} {}

    double yaw;
    double pitch;
    double roll;
    bool validity;

    virtual std::string toString() const override {
        std::stringstream ss;

        ss << yaw << DELIMITER << pitch << DELIMITER << roll;
        return ss.str();
    }
};

struct TelemetryReading : TimedData, ILoggable {
    TelemetryReading(long t, DataReading altitude, DataReading speed, DataReading acceleration,
                     DataReading pressure, DataReading temperature, YawPitchRollReading ypr) :
            TimedData(t), altitude{altitude}, speed{speed}, acceleration{acceleration}, pressure{pressure},
            temperature{temperature}, ypr{ypr} {}

    DataReading altitude;
    DataReading speed;
    DataReading acceleration;
    DataReading pressure;
    DataReading temperature;
    YawPitchRollReading ypr;

    virtual std::string toString() const override {
        std::stringstream ss;
        ss << this->timestamp << DELIMITER
           << altitude.value << DELIMITER
           << speed.value << DELIMITER
           << acceleration.value << DELIMITER
           << pressure.value << DELIMITER
           << temperature.value << DELIMITER
           << ypr.toString();

        return ss.str();
    }
};

#endif // DATASTRUCTS_H
