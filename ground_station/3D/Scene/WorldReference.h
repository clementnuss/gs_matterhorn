
#ifndef GS_MATTERHORN_WORLDREFERENCE_H
#define GS_MATTERHORN_WORLDREFERENCE_H

#include <QtGui/QVector2D>
#include <3D/CoordinateUtils.h>
#include <3D/Utils.h>
#include <cmath>
#include <3D/Grid/ContinuousElevationModel.h>

//TODO: make immutable
class WorldReference {
public:
    explicit WorldReference(const LatLon &origin) :
            origin_{origin},
            arcNorthSouthDistance_{
                    (2.0 * M_PI * GridConstants::EARTH_RADIUS) / (360 * GridConstants::SECONDS_PER_DEGREE)},
            arcWestEastDistance_{arcNorthSouthDistance_ * std::cos(toRadians(origin.latitude))} {
    }

    LatLon origin() const {
        return origin_;
    }

    double oneSecondArcLengthWE() const {
        return arcWestEastDistance_;
    }

    double oneSecondArcLengthNS() const {
        return arcNorthSouthDistance_;
    }

    double latitudeFromDistance(float meters, double reference) const {
        return reference + (meters / (arcNorthSouthDistance_ * GridConstants::SECONDS_PER_DEGREE));
    }

    double longitudeFromDistance(float meters, double reference) const {
        return reference + (meters / (arcWestEastDistance_ * GridConstants::SECONDS_PER_DEGREE));
    }

    QVector3D worldPosAt(const LatLon &point, std::shared_ptr<const ContinuousElevationModel> &model) const {
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
                                     * arcNorthSouthDistance_;
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
    LatLon origin_;
    double arcNorthSouthDistance_;
    double arcWestEastDistance_;
};


#endif //GS_MATTERHORN_WORLDREFERENCE_H

