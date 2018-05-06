
#include <ProgramConstants.h>
#include <DataStructures/Datastructs.h>
#include <Readers/FileReader.h>
#include <Utilities/ReaderUtils.h>
#include <3D/Ground/Ground.h>
#include "SplashDownPredictor.h"


SplashDownPredictor::SplashDownPredictor(const std::string &path, Qt3DCore::QNode *const parent) :
        predictionStrategy_{std::make_unique<AdaptiveSpeedPredictionStrategy>()},
        status_{},
        trajectory_{},
        trajectoryLine_{new Line(parent, QColor::fromRgb(240, 0, 0))},
        windData_{WindData::fromFile(path)} {
    status_.position = {0, 0, 0};
    status_.speed = {0, 0, 0};
    status_.acceleration = {0, 0, 0};
}

QVector3D
SplashDownPredictor::getTouchdownCoordinates(Ground *const ground) const {

    for (const QVector3D *it = trajectory_.end() - 1; it != trajectory_.begin(); it--) {
        float x = it->x();
        float z = it->z();

        float elev = ground->groundElevationAt(
                static_cast<int>(std::round(x)),
                static_cast<int>(std::round(z))
        );

        if (elev < it->y()) {
            return {x, elev, z};
        }
    }

    return {0, 0, 0};
}

void
SplashDownPredictor::updatePos(const QVector3D &pos) {
    status_.position = pos;
}

void
SplashDownPredictor::updateSpeed(const QVector3D &speed) {
    status_.speed = status_.speed * SplashDownPredictor::LEAKY_FACTOR + speed * (1 - SplashDownPredictor::LEAKY_FACTOR);
}

void
SplashDownPredictor::recomputePrediction() {
    trajectory_ = predictionStrategy_->predictTrajectory(status_, windData_);
    this->trajectoryLine_->setData(trajectory_);
}

const WindData *
SplashDownPredictor::windData() {
    return &windData_;
}
