
#ifndef GS_MATTERHORN_DISCRETEELEVATIONMODEL_H
#define GS_MATTERHORN_DISCRETEELEVATIONMODEL_H

#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

#include <3D/CoordinateUtils.h>
#include <DataStructures/Interval2D.h>

class DiscreteElevationModel {

public:
    explicit DiscreteElevationModel(std::string &filePath, GeoPoint &topLeftGeoPoint);

    static int geoAngleToIndex(const GeoAngle &geoAngle);

    float elevationAt(int latitudeIndex, int longitudeIndex) const;

    bool contains(int latitudeIndex, int longitudeIndex) const;

private:

    static constexpr int MODEL_RESOLUTION = 3600;
    static constexpr int BYTES_PER_MEASURE = 2;
    const uint16_t *hgtData_;
    boost::iostreams::mapped_file_source hgtFile_;
    Interval2D extent_;
};


#endif //GS_MATTERHORN_DISCRETEELEVATIONMODEL_H
