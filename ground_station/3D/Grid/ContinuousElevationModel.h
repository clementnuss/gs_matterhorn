
#ifndef GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
#define GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H


#include "DiscreteElevationModel.h"

class ContinuousElevationModel {

public:
    explicit ContinuousElevationModel(DiscreteElevationModel *);

    double elevationAt(const LatLon &latLon) const;

    float elevationSample(const int latitudeIndex, const int longitudeIndex) const;

private:
    DiscreteElevationModel *discreteModel_;
};


#endif //GS_MATTERHORN_CONTINUOUSELEVATIONMODEL_H
