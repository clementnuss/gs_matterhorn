
#ifndef GS_MATTERHORN_WORLDREFERENCE_H
#define GS_MATTERHORN_WORLDREFERENCE_H

#include <QtGui/QVector2D>
#include <3D/CoordinateUtils.h>
#include <3D/Utils.h>
#include <3D/Grid/ContinuousElevationModel.h>

class WorldReference {
public:
    explicit WorldReference(const LatLon &origin) :
            origin_{origin},
            arcWestEastDistance_{GridConstants::ARC_NORTH_SOUTH_DISTANCE * std::cos(toRadians(origin.latitude))} {}

    LatLon origin() {
        return origin_;
    }

    double latitudeFromDistance(float meters, double reference) const {
        return reference + (meters / (GridConstants::ARC_NORTH_SOUTH_DISTANCE * GridConstants::SAMPLES_PER_DEGREE));
    }

    double longitudeFromDistance(float meters, double reference) const {
        return reference + (meters / (arcWestEastDistance_ * GridConstants::SAMPLES_PER_DEGREE));
    }

    QVector3D worldPosAt(const LatLon &point, const ContinuousElevationModel *model) const {
        QVector2D v = translationFromOrigin(point);
        return {
                v.x(),
                static_cast<float>(model->elevationAt(point)),
                v.y()
        };
    }

    QVector2D translationFromOrigin(const LatLon &point) const {
        double latitudeTranslation = (point.latitude - origin_.latitude)
                                     * GridConstants::SECONDS_PER_DEGREE
                                     * GridConstants::ARC_NORTH_SOUTH_DISTANCE;
        double longitudeTranslation = (point.longitude - origin_.longitude)
                                      * GridConstants::SECONDS_PER_DEGREE
                                      * arcWestEastDistance_;

        return {
                static_cast<float>(latitudeTranslation),
                static_cast<float>(longitudeTranslation)
        };
    }

    LatLon latLonFromPointAndDistance(const LatLon &point, int metersAlongLatitude, int metersAlongLongitude) const {
        return {
                latitudeFromDistance(metersAlongLatitude, point.latitude),
                longitudeFromDistance(metersAlongLongitude, point.longitude)
        };
    }

private:
    const LatLon origin_;
    double arcWestEastDistance_;
};


#endif //GS_MATTERHORN_WORLDREFERENCE_H

