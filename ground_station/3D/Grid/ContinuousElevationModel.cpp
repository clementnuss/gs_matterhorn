
#include <3D/Utils.h>
#include <iostream>
#include "ContinuousElevationModel.h"

ContinuousElevationModel::ContinuousElevationModel(DiscreteElevationModel *discreteModel) : discreteModel_{
        discreteModel} {
}

float ContinuousElevationModel::elevationAt(const LatLon &latLon) const {

    GeoPoint geoPoint = latLonToGeoPoint(latLon);

    double lonSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    double latSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.latitude);

    int lonSWi = static_cast<int>(std::floor(lonSW));
    int latSWi = static_cast<int>(std::floor(latSW));

    double eleSW = elevationSample(latSWi, lonSWi);
    double eleNW = elevationSample(latSWi + 1, lonSWi);
    double eleSE = elevationSample(latSWi, lonSWi + 1);
    double eleNE = elevationSample(latSWi + 1, lonSWi + 1);

    double elevation = bilerp(eleSW, eleSE, eleNW, eleNE, lonSW - lonSWi, latSW - latSWi);

    return static_cast<float>(elevation);
}

float ContinuousElevationModel::elevationSample(const int latitudeIndex, const int longitudeIndex) const {

    return discreteModel_->contains(latitudeIndex, longitudeIndex) ? discreteModel_->elevationAt(latitudeIndex,
                                                                                                 longitudeIndex) : 0;
}
