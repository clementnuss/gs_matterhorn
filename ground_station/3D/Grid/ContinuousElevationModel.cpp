
#include <3D/Utils.h>
#include <iostream>
#include <3D/Scene/WorldReference.h>

ContinuousElevationModel::ContinuousElevationModel(std::unique_ptr<const IDiscreteElevationModel> discreteModel,
                                                   std::shared_ptr<const WorldReference> worldRef) : discreteModel_{
        std::move(discreteModel)}, worldRef_{std::move(worldRef)} {
}

float
ContinuousElevationModel::elevationAt(const LatLon &latLon) const {

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

QVector3D
ContinuousElevationModel::slopeAt(const LatLon &latLon) const {

    GeoPoint geoPoint = latLonToGeoPoint(latLon);

    double lonSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.longitude);
    double latSW = DiscreteElevationModel::geoAngleToIndex(geoPoint.latitude);

    int lonSWi = static_cast<int>(std::floor(lonSW));
    int latSWi = static_cast<int>(std::floor(latSW));

    QVector3D slopeSW = slopeSample(latSWi, lonSWi);
    QVector3D slopeNW = slopeSample(latSWi + 1, lonSWi);
    QVector3D slopeSE = slopeSample(latSWi, lonSWi + 1);
    QVector3D slopeNE = slopeSample(latSWi + 1, lonSWi + 1);

    QVector3D slope = bilerpVect(slopeSW, slopeSE, slopeNW, slopeNE, lonSW - lonSWi, latSW - latSWi);

    return slope;
}

float
ContinuousElevationModel::elevationSample(const int latitudeIndex, const int longitudeIndex) const {

    return discreteModel_->extent().contains(latitudeIndex, longitudeIndex) ?
           discreteModel_->elevationAt(latitudeIndex, longitudeIndex) : 0;
}

QVector3D
ContinuousElevationModel::slopeSample(const int latitudeIndex, const int longitudeIndex) const {
    double e = elevationSample(latitudeIndex, longitudeIndex);

    double arcN = worldRef_->oneSecondArcLengthNS();
    double arcW = worldRef_->oneSecondArcLengthWE();

    double dN = (e - elevationSample(latitudeIndex + 1, longitudeIndex));
    double dE = (e - elevationSample(latitudeIndex, longitudeIndex + 1));

    QVector3D vEast{
            0,
            static_cast<float>(dE),
            static_cast<float>(arcW)};
    QVector3D vNorth{
            static_cast<float>(arcN),
            static_cast<float>(dN),
            0};

    return QVector3D::crossProduct(vEast, vNorth).normalized();
}
