
#ifndef GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
#define GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H

#include "DiscreteElevationModel.h"

class WorldReference;

class ContinuousElevationModel {

public:
    explicit ContinuousElevationModel(const IDiscreteElevationModel *const,
                                      const WorldReference *const worldRef);

    //From https://cs108.epfl.ch/archive/17/p/03_elevation-models.html
    float elevationAt(const LatLon &latLon) const;

    QVector3D slopeAt(const LatLon &latLon) const;
    float elevationSample(const int latitudeIndex, const int longitudeIndex) const;

    QVector3D slopeSample(const int latitudeIndex, const int longitudeIndex) const;

private:
    const IDiscreteElevationModel *const discreteModel_;
    const WorldReference *const worldRef_;
};


#endif //GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
