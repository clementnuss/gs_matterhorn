
#ifndef GS_MATTERHORN_WORLDREFERENCE_H
#define GS_MATTERHORN_WORLDREFERENCE_H

#include <QtGui/QVector2D>
#include <3D/CoordinateUtils.h>
#include <3D/Utils.h>

static inline double metersAndRefToAngle(float meters, double reference, double arcLength) {
    return reference + (meters / (arcLength * GridConstants::SAMPLES_PER_DEGREE));
}

class WorldReference {
public:
    explicit WorldReference(const LatLon &origin) :
            origin_{origin},
            arcWestEastDistance_{GridConstants::ARC_NORTH_SOUTH_DISTANCE * std::cos(toRadians(origin.latitude))} {}

    LatLon origin() {
        return origin_;
    }

    QVector3D worldPosAt(const LatLon &point, const ContinuousElevationModel *model) {
        QVector2D v = translationFromOrigin(point);
        return {v.x(), static_cast<float>(model->elevationAt(point)), v.y()};
    }

    QVector2D translationFromOrigin(const LatLon &point) {
        double latitudeTranslation = (point.latitude - origin_.latitude) * GridConstants::SECONDS_PER_DEGREE *
                                     GridConstants::ARC_NORTH_SOUTH_DISTANCE;
        double longitudeTranslation =
                (point.longitude - origin_.longitude) * GridConstants::SECONDS_PER_DEGREE * arcWestEastDistance_;

        return {static_cast<float>(latitudeTranslation), static_cast<float>(longitudeTranslation)};
    }

    LatLon latLonFromPointAndDistance(const LatLon &point, int metersAlongLatitude, int metersAlongLongitude) {
        return {
                static_cast<float>(metersAndRefToAngle(metersAlongLatitude, point.latitude,
                                                       GridConstants::ARC_NORTH_SOUTH_DISTANCE)),
                static_cast<float>(metersAndRefToAngle(metersAlongLongitude, point.longitude, arcWestEastDistance_))
        };
    }

private:
    const LatLon origin_;
    double arcWestEastDistance_;
};


#endif //GS_MATTERHORN_WORLDREFERENCE_H

