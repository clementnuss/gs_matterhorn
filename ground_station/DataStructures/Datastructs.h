#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <utility>
#include <Flyable.h>
#include "ILoggable.h"
#include "ProgramConstants.h"

using namespace PrintConstants;

// Needed because of the C++ limitation in forward-declaring nested classes
class PacketDispatcher;

struct IDeserializable {
    virtual ~IDeserializable() = default;

    virtual bool isValid() const = 0;
};

struct DataPacket {
    DataPacket() = default;

    explicit DataPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType);

    virtual void dispatchWith(PacketDispatcher *);

    uint32_t timestamp_;
    uint32_t sequenceNumber_;
    FlyableType flyableType_;
};

struct EventPacket : DataPacket, ILoggable, IDeserializable {
    EventPacket() = default;
    EventPacket(const EventPacket &that) = default;
    EventPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                int code, const std::string &description);
    ~EventPacket() = default;

    int code_{};
    std::string description_;

    void dispatchWith(PacketDispatcher *) override;

    std::string toString() const override;

    bool isValid() const override;
};

struct ControlPacket : DataPacket, IDeserializable {
    ControlPacket() = default;
    ControlPacket(const ControlPacket &that) = default;
    ControlPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                  uint8_t partCode, uint16_t statusValue);

    uint8_t partCode_;
    uint16_t statusValue_;

    void dispatchWith(PacketDispatcher *) override;

    bool isValid() const override;
};

struct GPSPacket : DataPacket, ILoggable, IDeserializable {
    GPSPacket() = default;
    GPSPacket(const GPSPacket &that) = default;
    GPSPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
              uint8_t satsCount, float hdop, float latitude, float longitude, float altitude);

    uint8_t satsCount_;
    float hdop_;
    float latitude_;
    float longitude_;
    float altitude_;

    std::string toString() const override;

    void dispatchWith(PacketDispatcher *) override;

    bool isValid() const override;

    LatLon latLon() const;
};

struct Data3D : ILoggable {
    Data3D();

    Data3D(double x, double y, double z);

    double x_;
    double y_;
    double z_;

    std::string toString() const override;

    double norm() const;

    Data3D operator+(Data3D &rhs) const;

    Data3D operator*=(double coeff);

    Data3D operator+=(const Data3D &rhs);
};

struct SensorsPacket : DataPacket, ILoggable, IDeserializable {
    SensorsPacket();
    SensorsPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                  double altitude, Data3D acceleration,
                  Data3D magnetometer, Data3D gyroscope,
                  double pressure, double temperature, double air_speed);
    SensorsPacket(const SensorsPacket &that) = default;
    ~SensorsPacket() override = default;

    Data3D acceleration_;
    Data3D eulerAngles_;
    Data3D gyroscope_;
    double altitude_;
    double pressure_;
    double temperature_;
    double air_speed_;

    void dispatchWith(PacketDispatcher *) override;

    std::string toString() const override;

    SensorsPacket operator+=(const SensorsPacket &rhs);

    SensorsPacket operator*=(double coeff);

    bool isValid() const override;
};


#endif // DATASTRUCTS_H
