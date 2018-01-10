
#include <3D/Utils.h>
#include <iostream>
#include <3D/Scene/WorldReference.h>

ContinuousElevationModel::ContinuousElevationModel(const DiscreteElevationModel *const discreteModel,
                                                   const WorldReference *const worldRef) : discreteModel_{
        discreteModel}, worldRef_{worldRef} {
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

float ContinuousElevationModel::slopeAt(const LatLon &latLon) const {

    GeoPoint geoPoint = latLonToGeoPoint(latLon);

    double lonSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    double latSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.latitude);

    int lonSWi = static_cast<int>(std::floor(lonSW));
    int latSWi = static_cast<int>(std::floor(latSW));

    double eleSW = slopeSample(latSWi, lonSWi);
    double eleNW = slopeSample(latSWi + 1, lonSWi);
    double eleSE = slopeSample(latSWi, lonSWi + 1);
    double eleNE = slopeSample(latSWi + 1, lonSWi + 1);

    double elevation = bilerp(eleSW, eleSE, eleNW, eleNE, lonSW - lonSWi, latSW - latSWi);

    return static_cast<float>(elevation);
}

float ContinuousElevationModel::elevationSample(const int latitudeIndex, const int longitudeIndex) const {

    return discreteModel_->contains(latitudeIndex, longitudeIndex) ? discreteModel_->elevationAt(latitudeIndex,
                                                                                                 longitudeIndex) : 0;
}

float ContinuousElevationModel::slopeSample(const int latitudeIndex, const int longitudeIndex) const {
    double e = elevationSample(latitudeIndex, longitudeIndex);

    double arcN = worldRef_->oneSecondArcLengthNS();
    double arcW = worldRef_->oneSecondArcLengthWE();
    double arcNSq = std::pow(arcN, 2);
    double arcWSq = std::pow(arcW, 2);
    double dESq = std::pow((e - elevationSample(latitudeIndex + 1, longitudeIndex)), 2);
    double dNSq = std::pow((e - elevationSample(latitudeIndex, longitudeIndex + 1)), 2);

    double theta = std::acos(
            (arcN * arcW)
            / sqrt(
                    dESq * arcNSq
                    + dNSq * arcWSq
                    + arcNSq * arcWSq
            )
    );

    std::cout << "theta " << theta << " ";

    return static_cast<float>(theta);
}
