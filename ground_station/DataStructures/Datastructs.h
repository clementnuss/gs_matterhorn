#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <utility>
#include <Flyable.h>
#include "ProgramConstants.h"

using namespace PrintConstants;

class PacketDispatcher;

struct IDeserializable {
    virtual ~IDeserializable() = default;

    virtual bool isValid() const = 0;
};

struct ILoggable {
    virtual std::string toString() const = 0;
};

struct IDispatchable : public ILoggable {
    virtual void dispatchWith(PacketDispatcher *);
};

struct DataPacket : public IDispatchable {
    DataPacket() = default;

    explicit DataPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType);

    void dispatchWith(PacketDispatcher *) override;

    std::string toString() const override;

    uint32_t timestamp_;
    uint32_t sequenceNumber_;
    FlyableType flyableType_;
};

struct ATCommandResponse : public IDispatchable {

    static constexpr uint8_t FRAME_DELIMITER = 0x7E;
    static constexpr uint8_t FRAME_TYPE = 0x88;
    static constexpr size_t HEADER_SIZE = 2;
    static constexpr size_t PAYLOAD_SIZE = 6;
    static constexpr size_t CHECKSUM_SIZE = 1;
    static constexpr uint16_t RSSI_COMMAND_CODE = 0x4442;
    static const std::vector<uint8_t> RSSI_COMMAND;

    ATCommandResponse() = default;

    explicit ATCommandResponse(uint8_t, uint16_t, uint8_t);

    uint8_t frameID_;
    uint16_t command_;
    uint8_t status_;

    void dispatchWith(PacketDispatcher *) override;

    std::string toString() const override;
};

struct RSSIResponse : public ATCommandResponse {

    RSSIResponse() = default;

    explicit RSSIResponse(uint8_t, uint16_t, uint8_t, uint8_t);
    void dispatchWith(PacketDispatcher *) override;
    std::string toString() const override;

    uint8_t value_;
};

struct EventPacket : public DataPacket, IDeserializable {
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

struct ControlPacket : public DataPacket, IDeserializable {
    ControlPacket() = default;
    ControlPacket(const ControlPacket &that) = default;
    ControlPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                  uint8_t partCode, uint16_t statusValue);

    uint8_t partCode_;
    uint16_t statusValue_;

    void dispatchWith(PacketDispatcher *) override;

    bool isValid() const override;
};

struct GPSPacket : public DataPacket, IDeserializable {
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

struct Data3D : public ILoggable {
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

struct SensorsPacket : public DataPacket, IDeserializable {
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
