#include "DataStructures/Datastructs.h"
#include "PacketDispatcher.h"

void
IDispatchable::dispatchWith(PacketDispatcher *) {

}

/**
 * DataPacket
 */

DataPacket::DataPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType) :
        timestamp_{timestamp},
        sequenceNumber_{sequenceNumber},
        flyableType_{flyableType} {}

void
DataPacket::dispatchWith(PacketDispatcher *d) {

}

std::string
DataPacket::toString() const {
    std::stringstream ss;

    ss << static_cast<int>(flyableType_)
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << sequenceNumber_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << timestamp_;

    return ss.str();
}

/**
 *  AT command response
 */
void
ATCommandResponse::dispatchWith(PacketDispatcher *d) {

}

std::string
ATCommandResponse::toString() const {
    std::stringstream ss;

    ss << command_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << frameType_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << frameID_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << status_;

    return ss.str();
}

ATCommandResponse::ATCommandResponse(uint8_t frameType, uint8_t frameID, uint16_t command, uint8_t status) :
        frameType_{frameType}, frameID_{frameID}, command_{command}, status_{status} {

}

RSSIResponse::RSSIResponse(uint8_t frameType, uint8_t frameID, uint16_t command, uint8_t status, uint8_t value) :
        ATCommandResponse{frameType, frameID, command, status}, value_{value} {}

void
RSSIResponse::dispatchWith(PacketDispatcher *d) {
    d->dispatch(this);
}

std::string
RSSIResponse::toString() const {
    std::stringstream ss;

    ss << ATCommandResponse::toString()
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << value_;

    return ss.str();
}


/**
 * EventPacket
 */
EventPacket::EventPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                         int code, const std::string &description) :
        DataPacket{timestamp, sequenceNumber, flyableType}, code_{code}, description_{std::move(description)} {}


void
EventPacket::dispatchWith(PacketDispatcher *d) {

}

std::string
EventPacket::toString() const {
    std::stringstream ss;

    ss << DataPacket::toString()
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << code_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << " "
       << description_;

    return ss.str();
}

bool
EventPacket::isValid() const {
    return false;
}


/**
 * ControlPacket
 */

ControlPacket::ControlPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                             uint8_t partCode, uint16_t statusValue) :
        DataPacket{timestamp, sequenceNumber, flyableType}, partCode_{partCode}, statusValue_{statusValue} {}


void
ControlPacket::dispatchWith(PacketDispatcher *d) {

}

bool
ControlPacket::isValid() const {
    return false;
}


/**
 * GPSPacket
 */

GPSPacket::GPSPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                     uint8_t satsCount, float hdop, float latitude, float longitude, float altitude) :
        DataPacket{timestamp, sequenceNumber, flyableType},
        satsCount_{satsCount},
        hdop_{hdop},
        latitude_{latitude},
        longitude_{longitude},
        altitude_{altitude} {}


std::string
GPSPacket::toString() const {
    std::stringstream ss;

    ss << DataPacket::toString()
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << (int) satsCount_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << hdop_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION_GPS) << fixed << latitude_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION_GPS) << fixed << longitude_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << altitude_;

    return ss.str();
}

void
GPSPacket::dispatchWith(PacketDispatcher *d) {
    d->dispatch(this);
}

bool
GPSPacket::isValid() const {
    return (-90.0f <= latitude_ && latitude_ <= 90.0f)
           &
           (-180.0f <= longitude_ && longitude_ <= 180.0f);
}

LatLon
GPSPacket::latLon() const {
    return LatLon{latitude_, longitude_};
}


/**
 * Data3D
 */
Data3D::Data3D() : x_{0}, y_{0}, z_{0} {};

Data3D::Data3D(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}


std::string
Data3D::toString() const {
    std::stringstream ss;

    ss << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << x_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << y_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << z_;

    return ss.str();
}

double
Data3D::norm() const {
    return std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
}

Data3D
Data3D::operator+(Data3D &rhs) const {
    Data3D result{};
    result.x_ = this->x_ + rhs.x_;
    result.y_ = this->y_ + rhs.y_;
    result.z_ = this->z_ + rhs.z_;
    return result;
}

Data3D
Data3D::operator*=(double coeff) {
    x_ *= coeff;
    y_ *= coeff;
    z_ *= coeff;

}

Data3D
Data3D::operator+=(const Data3D &rhs) {
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
}


/**
 * SensorsPacket
 */
SensorsPacket::SensorsPacket() : DataPacket{0, 0, FlyableType::ROCKET},
                                 altitude_{0},
                                 acceleration_{},
                                 eulerAngles_{},
                                 gyroscope_{},
                                 pressure_{0},
                                 temperature_{0},
                                 air_speed_{0} {};

SensorsPacket::SensorsPacket(uint32_t timestamp, uint32_t sequenceNumber, FlyableType flyableType,
                             double altitude, Data3D acceleration,
                             Data3D magnetometer, Data3D gyroscope,
                             double pressure, double temperature, double air_speed) :
        DataPacket{timestamp, sequenceNumber, flyableType},
        altitude_{altitude},
        acceleration_{acceleration},
        eulerAngles_{magnetometer},
        gyroscope_{gyroscope},
        pressure_{pressure},
        temperature_{temperature},
        air_speed_{air_speed} {}


void
SensorsPacket::dispatchWith(PacketDispatcher *d) {
    d->dispatch(this);
}

std::string
SensorsPacket::toString() const {

    std::stringstream ss;
    ss << DataPacket::toString()
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << altitude_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << air_speed_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << pressure_
       << std::setw(FIELD_WIDTH) << std::setfill(DELIMITER) << std::setprecision(PRECISION) << fixed << temperature_
       << acceleration_.toString()
       << gyroscope_.toString()
       << eulerAngles_.toString();

    return ss.str();
}

SensorsPacket
SensorsPacket::operator+=(const SensorsPacket &rhs) {
    air_speed_ += rhs.air_speed_;
    pressure_ += rhs.pressure_;
    temperature_ += rhs.temperature_;
    altitude_ += rhs.altitude_;
    acceleration_ += rhs.acceleration_;
    eulerAngles_ += rhs.eulerAngles_;
    gyroscope_ += rhs.gyroscope_;
}


SensorsPacket
SensorsPacket::operator*=(double coeff) {
    air_speed_ *= coeff;
    pressure_ *= coeff;
    temperature_ *= coeff;
    altitude_ *= coeff;
    acceleration_ *= coeff;
    eulerAngles_ *= coeff;
    gyroscope_ *= coeff;
}

bool
SensorsPacket::isValid() const {
    return false;
}


