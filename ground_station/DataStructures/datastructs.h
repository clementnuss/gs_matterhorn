#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
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

struct XYZReading : ILoggable {
    XYZReading() = default;

    XYZReading(double x, double y, double z, bool b) : x{x}, y{y}, z{z}, validity{b} {}

    double x;
    double y;
    double z;
    bool validity;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << x
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << y
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << z;

        return ss.str();
    }

    double norm() {
        return std::sqrt(x * x + y * y + z * z);
    }
};

struct TelemetryReading : TimedData, ILoggable {
    TelemetryReading() = default;

    /**
     *
     * @param t
     * @param altitude
     * @param acceleration
     * @param magnetometer
     * @param gyroscope
     * @param pressure
     * @param temperature
     */
    TelemetryReading(long t, DataReading altitude, /*DataReading speed,*/ XYZReading acceleration,
                     XYZReading magnetometer, XYZReading gyroscope, DataReading pressure, DataReading temperature) :
            TimedData(t), altitude{altitude}, /*speed{speed},*/ acceleration{acceleration}, magnetometer{magnetometer},
            gyroscope{gyroscope}, pressure{pressure}, temperature{temperature} {}

    TelemetryReading(const TelemetryReading &that) = default;

    ~TelemetryReading() = default;

    DataReading altitude;
//    DataReading speed;
    XYZReading acceleration;
    XYZReading magnetometer;
    XYZReading gyroscope;
    DataReading pressure;
    DataReading temperature;

    string toString() const override {
        stringstream format;
        format << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed;

        stringstream ss;
        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << this->timestamp
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << altitude.value
           //           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << speed.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << pressure.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << temperature.value
           << acceleration.toString()
           << gyroscope.toString()
           << magnetometer.toString();

        return ss.str();
    }
};

#endif // DATASTRUCTS_H
