
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

    float elevationAt(int x, int y) const;

    bool contains(int x, int y) const;

private:

    static constexpr int MODEL_RESOLUTION = 3600;
    static constexpr int BYTES_PER_MEASURE = 2;
    const int16_t *hgtData_;
    Interval2D extent_;
};


#endif //GS_MATTERHORN_DISCRETEELEVATIONMODEL_H
