
#ifndef GS_MATTERHORN_COORDINATEUTILS_H
#define GS_MATTERHORN_COORDINATEUTILS_H

#include <cmath>

struct LatLon {
    double latitude;
    double longitude;
};

struct GeoAngle {
    int degrees;
    int minutes;
    int seconds;
};

struct GeoPoint {
    GeoAngle latitude;
    GeoAngle longitude;
};


static GeoPoint latLonToGeoPoint(const LatLon &latLon) {

    GeoAngle latitude{};
    GeoAngle longitude{};

    latitude.degrees = static_cast<int>(std::floor(latLon.latitude));
    longitude.degrees = static_cast<int>(std::floor(latLon.longitude));


    double minutesLat = 60 * (latLon.latitude - latitude.degrees);
    double minutesLon = 60 * (latLon.longitude - longitude.degrees);
    latitude.minutes = static_cast<int>(std::floor(minutesLat));
    longitude.minutes = static_cast<int>(std::floor(minutesLon));

    latitude.seconds = static_cast<int>(std::floor(minutesLat - latitude.minutes));
    longitude.seconds = static_cast<int>(std::floor(minutesLon - longitude.minutes));

    return {latitude, longitude};
}

#endif //GS_MATTERHORN_COORDINATEUTILS_H
