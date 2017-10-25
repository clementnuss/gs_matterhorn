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

struct IDeserializable {
    virtual ~IDeserializable() = default;
};

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

    XYZReading(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

    double x_;
    double y_;
    double z_;

    string toString() const override {
        stringstream ss;

        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << x_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << y_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << z_;

        return ss.str();
    }

    double norm() {
        return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    }
};

struct TelemetryReading : TimedData, ILoggable, IDeserializable {
    TelemetryReading() = default;

    TelemetryReading(long t, double altitude, XYZReading acceleration,
                     XYZReading magnetometer, XYZReading gyroscope,
                     double pressure, double temperature) :
            TimedData(t),
            altitude_{altitude},
            acceleration_{acceleration},
            magnetometer_{magnetometer},
            gyroscope_{gyroscope},
            pressure_{pressure},
            temperature_{temperature} {}

    TelemetryReading(const TelemetryReading &that) = default;

    ~TelemetryReading() = default;

    XYZReading acceleration_;
    XYZReading magnetometer_;
    XYZReading gyroscope_;
    double altitude_;
    double pressure_;
    double temperature_;

    string toString() const override {
        stringstream format;
        format << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed;

        stringstream ss;
        ss << setw(FIELD_WIDTH) << setfill(DELIMITER) << this->timestamp
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << altitude_
           //           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << speed.value
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << pressure_
           << setw(FIELD_WIDTH) << setfill(DELIMITER) << setprecision(PRECISION) << fixed << temperature_
           << acceleration_.toString()
           << gyroscope_.toString()
           << magnetometer_.toString();

        return ss.str();
    }
};

#endif // DATASTRUCTS_H
