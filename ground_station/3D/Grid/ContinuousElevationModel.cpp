
#include <3D/Utils.h>
#include "ContinuousElevationModel.h"

ContinuousElevationModel::ContinuousElevationModel(DiscreteElevationModel *discreteModel) : discreteModel_{
        discreteModel} {

}

double ContinuousElevationModel::elevationAt(const LatLon &latLon) const {
    GeoPoint geoPoint = latLonToGeoPoint(latLon);

    int lonSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    int latSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    double eleSW = elevationSample(lonSW, latSW);
    double eleNW = elevationSample(lonSW, latSW + 1);
    double eleSE = elevationSample(lonSW + 1, latSW);
    double eleNE = elevationSample(lonSW + 1, latSW + 1);

    return bilerp(eleSW, eleSE, eleNW, eleNE, latLon.longitude - lonSW, latLon.latitude - latSW);
}

double ContinuousElevationModel::elevationSample(const int x, const int y) const {
    return discreteModel_->contains(x, y) ? discreteModel_->elevationAt(x, y) : 0;
}
