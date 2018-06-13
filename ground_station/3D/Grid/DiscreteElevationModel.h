
#ifndef GS_MATTERHORN_DISCRETEELEVATIONMODEL_H
#define GS_MATTERHORN_DISCRETEELEVATIONMODEL_H

#include <string>
#include <boost/iostreams/device/mapped_file.hpp>

#include <3D/CoordinateUtils.h>
#include <DataStructures/Interval2D.h>
#include <3D/3DVisualisationConstants.h>
#include "IDiscreteElevationModel.h"

class DiscreteElevationModel : public IDiscreteElevationModel {

public:
    explicit DiscreteElevationModel(const std::string &filePath, const GeoPoint &topLeftGeoPoint);

    static double geoAngleToIndex(const GeoAngle &geoAngle);

    virtual float elevationAt(int latitudeIndex, int longitudeIndex) const;

    Interval2D extent() const override;

private:

    static constexpr int MODEL_RESOLUTION = GridConstants::SAMPLES_PER_DEGREE;
    static constexpr int BYTES_PER_MEASURE = 2;
    const uint16_t *hgtData_;
    boost::iostreams::mapped_file_source hgtFile_;
    Interval2D extent_;
};


#endif //GS_MATTERHORN_DISCRETEELEVATIONMODEL_H
