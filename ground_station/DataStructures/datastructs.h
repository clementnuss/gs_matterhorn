#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include "ILoggable.h"
#include "ProgramConstants.h"

using namespace std;
using namespace PrintConstants;

struct TimedData {
    TimedData() = default;
    TimedData(long t) : timestamp{t} {}

    long timestamp;
};

struct DataReading {
    DataReading() = default;
    DataReading(double v, bool b) : value{v}, validity{b} {}

    double value;
    bool validity;
};

struct RocketEvent : TimedData, ILoggable {
    //TODO: rule of three
    RocketEvent() = default;

    RocketEvent(const RocketEvent &that) = default;

    RocketEvent(long timestamp, int code, std::string description) :
            TimedData(timestamp), code{code}, description{description} {}

    ~RocketEvent() = default;

    int code;
    std::string description;

    virtual string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << timestamp
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << code
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << " "
           << description;

        return ss.str();
    }
};

struct YawPitchRollReading : ILoggable {
    YawPitchRollReading() = default;
    YawPitchRollReading(double y, double p, double r, bool b) : yaw{y}, pitch{p}, roll{r}, validity{b} {}

    double yaw;
    double pitch;
    double roll;
    bool validity;

    virtual string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << yaw
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << pitch
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << roll;
        
        return ss.str();
    }
};

struct TelemetryReading : TimedData, ILoggable {
    TelemetryReading() = default;

    TelemetryReading(long t, DataReading altitude, DataReading speed, DataReading acceleration,
                     DataReading pressure, DataReading temperature, YawPitchRollReading ypr) :
            TimedData(t), altitude{altitude}, speed{speed}, acceleration{acceleration}, pressure{pressure},
            temperature{temperature}, ypr{ypr} {}

    TelemetryReading(const TelemetryReading &that) = default;

    ~TelemetryReading() = default;

    DataReading altitude;
    DataReading speed;
    DataReading acceleration;
    DataReading pressure;
    DataReading temperature;
    YawPitchRollReading ypr;

    virtual string toString() const override {
        stringstream format;
        format << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed;

        stringstream ss;
        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << this->timestamp
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << altitude.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << speed.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << acceleration.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << pressure.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << temperature.value
           << ypr.toString();

        return ss.str();
    }
};

#endif // DATASTRUCTS_H
