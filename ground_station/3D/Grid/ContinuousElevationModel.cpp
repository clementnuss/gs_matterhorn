
#include <3D/Utils.h>
#include <iostream>
#include "ContinuousElevationModel.h"

ContinuousElevationModel::ContinuousElevationModel(DiscreteElevationModel *discreteModel) : discreteModel_{
        discreteModel} {

}

double ContinuousElevationModel::elevationAt(const LatLon &latLon) const {

    GeoPoint geoPoint = latLonToGeoPoint(latLon);

    int lonSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    int latSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.latitude);
    double eleSW = elevationSample(latSW, lonSW);
    double eleNW = elevationSample(latSW + 1, lonSW);
    double eleSE = elevationSample(latSW, lonSW + 1);
    double eleNE = elevationSample(latSW + 1, lonSW + 1);

    std::cout << "Elevation are: " << eleSW << " " << eleNW << " " << eleSE << " " << eleNE << std::endl;

    double elevation = eleSW; //TODO: use bilerp(eleSW, eleSE, eleNW, eleNE, latLon.longitude - lonSW, latLon.latitude - latSW);

    std::cout << "Final elevation: " << elevation;

    return elevation;
}

float ContinuousElevationModel::elevationSample(const int latitudeIndex, const int longitudeIndex) const {

    return discreteModel_->contains(latitudeIndex, longitudeIndex) ? discreteModel_->elevationAt(latitudeIndex,
                                                                                                 longitudeIndex) : 0;
}
