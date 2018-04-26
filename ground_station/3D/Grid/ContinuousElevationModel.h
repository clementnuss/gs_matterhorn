
#ifndef GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
#define GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H

#include "DiscreteElevationModel.h"

class WorldReference;

class ContinuousElevationModel {

public:
    explicit ContinuousElevationModel(std::unique_ptr<const IDiscreteElevationModel>,
                                      std::shared_ptr<const WorldReference>);

    //From https://cs108.epfl.ch/archive/17/p/03_elevation-models.html
    float elevationAt(const LatLon &latLon) const;

    QVector3D slopeAt(const LatLon &latLon) const;

    float elevationSample(const int latitudeIndex, const int longitudeIndex) const;

    QVector3D slopeSample(const int latitudeIndex, const int longitudeIndex) const;

private:
    std::unique_ptr<const IDiscreteElevationModel> discreteModel_;
    std::shared_ptr<const WorldReference> worldRef_;
};


#endif //GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
